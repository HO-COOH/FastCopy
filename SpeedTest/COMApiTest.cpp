#include "COMApiTest.h"
#include "COMInitializeHelper.h"

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

            wil::com_ptr<IShellItem> psiDest;
            if (!SUCCEEDED(SHCreateItemFromParsingName(convert_to_wstring(test.destination).data(), NULL, IID_PPV_ARGS(psiDest.put()))))
                return false;

            wil::com_ptr<IShellItem> psiFrom;
            if (!SUCCEEDED(SHCreateItemFromParsingName(convert_to_wstring(test.source).data(), NULL, IID_PPV_ARGS(psiFrom.put()))))
                return false;


            if (test.isFolder())
            {
                //TODO: Use CopyItems and IShellFolder to copy folders

                //wil::com_ptr<IShellFolder> psiFromFolder;
                //psiFrom->BindToHandler(nullptr, BHID_SFObject, IID_PPV_ARGS(psiFromFolder.put()));

                //wil::com_ptr<IShellItemArray> psiFromContentArray;
                //
                //PITEMID_CHILD rgpidlChildren[100] = { 0 };
                //psiFromFolder->
                //auto hr = SHCreateShellItemArray(nullptr, psiFromFolder.get(), 100, (LPCITEMIDLIST*)rgpidlChildren, psiFromContentArray.put());
                //if (!SUCCEEDED(hr))
                //    return false;
                //DWORD count;
                //psiFromContentArray->GetCount(&count);

                //return performOpImpl(psiFromContentArray.get(), psiDest.get(), test.operation);
            }
            else
            {
                return performOpImpl(psiFrom.get(), psiDest.get(), test.operation);
            }

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

wil::com_ptr<IFileOperation> COMApiTest::getOp()
{
    wil::com_ptr<IFileOperation> pfo;
    auto hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(pfo.put()));
    pfo->SetOperationFlags(FOF_NO_UI);
    return pfo;
}

bool COMApiTest::copyItemImpl(IShellItemArray* from, IShellItem* to)
{
    auto op = getOp();
    auto hr = op->CopyItems(from, to);
    auto hr2 = op->PerformOperations();
    return true;
}

bool COMApiTest::copyItemImpl(IShellItem* from, IShellItem* to)
{
    auto op = getOp();
    return SUCCEEDED(op->CopyItem(from, to, nullptr, nullptr)) && SUCCEEDED(op->PerformOperations());
}
