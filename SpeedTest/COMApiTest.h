#pragma once
#include "ICopyBase.h"
#include "COMInitializeHelper.h"
#include "AutoRegister.h"
#include <wil/com.h>

struct IFileOperation;
struct IShellItemArray;
struct IShellItem;

class COMApiTest : public IImplementationBase, private AutoRegister<COMApiTest>
{
public:
	COMApiTest();

	// Inherited via ICopyBase
	virtual bool Run(std::vector<TestOperation> const& paths) override;

	virtual std::string GetName() const override;
private:
	static void initCOM();

	static wil::com_ptr<IFileOperation> getOp();

	static bool copyItemImpl(IShellItemArray* from, IShellItem* to);
	static bool copyItemImpl(IShellItem* from, IShellItem* to);

	static bool moveItemImpl(IShellItemArray* from, IShellItem* to) { return{}; }
	static bool moveItemImpl(IShellItem* from, IShellItem* to) { return {}; }

	static bool performOpImpl(auto* from, auto* to, TestOperation::Operation op)
	{
		switch (op)
		{
		case TestOperation::Operation::Copy:
			return copyItemImpl(from, to);
		case TestOperation::Operation::Move:
			return moveItemImpl(from, to);
		default:
			return false;
		}
	}
};

