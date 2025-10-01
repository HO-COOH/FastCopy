#include "COMApiTest.h"
#include "COMInitializeHelper.h"

#include <ShlObj_core.h>
#include <execution>
#include <filesystem>
#include <cassert>
#include <codecvt>
#include "ShellItem.h"

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

            //ShellItem psiFrom{ test.isFolder()? test.source + L"\\*" : test.source};
			ShellItem psiFrom{ test.source };
            ShellItem psiDest{ test.destination };

			performOpImpl(psiFrom.GetPtr(), psiDest.GetPtr(), test.operation);
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
    THROW_IF_FAILED(CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(pfo.put())));
    pfo->SetOperationFlags(FOF_NO_UI);
    return pfo;
}

void COMApiTest::copyItemImpl(IShellItemArray* from, IShellItem* to)
{
    auto op = getOp();
    THROW_IF_FAILED(op->CopyItems(from, to));
    THROW_IF_FAILED(op->PerformOperations());
}

void COMApiTest::copyItemImpl(IShellItem* from, IShellItem* to)
{
    auto op = getOp();
    THROW_IF_FAILED(op->CopyItem(from, to, nullptr, nullptr));
    THROW_IF_FAILED(op->PerformOperations());
}

void COMApiTest::moveItemImpl(IShellItemArray* from, IShellItem* to)
{
    auto op = getOp();
	THROW_IF_FAILED(op->MoveItems(from, to));
    THROW_IF_FAILED(op->PerformOperations());
}

void COMApiTest::moveItemImpl(IShellItem* from, IShellItem* to)
{
    auto op = getOp();
    THROW_IF_FAILED(op->MoveItem(from, to, nullptr, nullptr));
	THROW_IF_FAILED(op->PerformOperations());
}

void COMApiTest::deleteItemImpl(IShellItemArray* from)
{
    auto op = getOp();
    THROW_IF_FAILED(op->DeleteItems(from));
	THROW_IF_FAILED(op->PerformOperations());
}

void COMApiTest::deleteItemImpl(IShellItem* from)
{
    auto op = getOp();
    THROW_IF_FAILED(op->DeleteItem(from, nullptr));
	THROW_IF_FAILED(op->PerformOperations());
}

void COMApiTest::performOpImpl(auto* from, auto* to, TestOperation::Operation op)
{
    switch (op)
    {
        case TestOperation::Operation::Copy:
            return copyItemImpl(from, to);
        case TestOperation::Operation::Move:
            return moveItemImpl(from, to);
		case TestOperation::Operation::Delete:
			return deleteItemImpl(from);
    }
}
