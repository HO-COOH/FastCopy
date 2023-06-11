#include "COMApiTest.h"
#include "COMInitializeHelper.h"
#include <wil/com.h>
#include <ShlObj_core.h>
#include <execution>
#include <filesystem>
#include <cassert>

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
            if(!SUCCEEDED(SHCreateItemFromParsingName(test.source.data(), NULL, IID_PPV_ARGS(psiFrom.put()))))
                return false;

            if (!SUCCEEDED(SHCreateItemFromParsingName(test.destination.data(), NULL, IID_PPV_ARGS(psiDest.put()))))
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
