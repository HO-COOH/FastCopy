#pragma once
#include "ITestBase.h"
#include "COMInitializeHelper.h"
#include "AutoRegister.h"
class COMApiTest : public ITestBase, private AutoRegister<COMApiTest>
{
public:
	COMApiTest();

	// Inherited via ITestBase
	virtual bool Run(std::vector<TestOperation> const& paths) override;

	virtual std::string GetName() const override;
private:
	static void initCOM();
};

