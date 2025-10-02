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

	static void copyItemImpl(IShellItemArray* from, IShellItem* to);
	static void copyItemImpl(IShellItem* from, IShellItem* to, LPCWSTR name);

	static void moveItemImpl(IShellItemArray* from, IShellItem* to);
	static void moveItemImpl(IShellItem* from, IShellItem* to, LPCWSTR name);

	static void deleteItemImpl(IShellItemArray* from);
	static void deleteItemImpl(IShellItem* from);

	static void performOpImpl(auto* from, auto* to, TestOperation::Operation op);
	static void performOpImpl(auto* from, auto* to, LPCWSTR name, TestOperation::Operation op);
};

