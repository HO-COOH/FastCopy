#include "COMApiTest.h"
#include "COMInitializeHelper.h"
#include <wil/com.h>
#include <ShlObj_core.h>
#include <execution>
#include <filesystem>
#include <cassert>
#include <codecvt>

//copied from https://stackoverflow.com/questions/42793735/how-to-convert-between-widecharacter-and-multi-byte-character-string-in-windows
std::wstring convert_to_wstring(const std::string& str)
{
    int num_chars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstrTo;
    if (num_chars)
    {
        wstrTo.resize(num_chars);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstrTo[0], num_chars);
    }
    return wstrTo;
}


COMApiTest::COMApiTest()
{
    initCOM();
}

bool COMApiTest::Run(std::vector<TestOperation> const& paths)
{
    return std::all_of(
        std::execution::par_unseq,
        std::begin(paths),
        std::end(paths),
        [](TestOperation const& test)
        {
            COMInitializeHelper s_helper;
            wil::com_ptr<IFileOperation> pfo;
            auto hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(pfo.put()));
            if (!SUCCEEDED(hr))
                return false;

            if (!SUCCEEDED(pfo->SetOperationFlags(FOF_NO_UI)))
                return false;

            wil::com_ptr<IShellItem> psiFrom, psiDest;
            if(!SUCCEEDED(SHCreateItemFromParsingName(convert_to_wstring(test.source).data(), NULL, IID_PPV_ARGS(psiFrom.put()))))
                return false;

            if (!SUCCEEDED(SHCreateItemFromParsingName(convert_to_wstring(test.source).data(), NULL, IID_PPV_ARGS(psiDest.put()))))
                return false;


            //if (!SUCCEEDED(pfo->CopyItem(psiFrom.get(), psiDest.get(), nullptr, nullptr)))
            //    return false;
            switch (test.operation)
            {
                case TestOperation::Operation::Copy:
                    if (!SUCCEEDED(pfo->CopyItem(psiFrom.get(), psiDest.get(), nullptr, nullptr)))
                        return false;
                case TestOperation::Operation::Move:
                    if (!SUCCEEDED(pfo->MoveItem(psiFrom.get(), psiDest.get(), nullptr, nullptr)))
                        return false;
            }


            if (!SUCCEEDED(pfo->PerformOperations()))
                return false;

            return true;
        }
    );
}

std::string COMApiTest::GetName() const
{
    return "COM IFileOperation";
}

void COMApiTest::initCOM()
{
    static COMInitializeHelper s_helper;
}
