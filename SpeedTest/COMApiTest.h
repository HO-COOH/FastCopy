#pragma once
#include "ICopyBase.h"
#include "COMInitializeHelper.h"
#include "AutoRegister.h"
class COMApiTest : public IImplementationBase, private AutoRegister<COMApiTest>
{
public:
	COMApiTest();

	// Inherited via ICopyBase
	virtual bool Run(std::vector<TestOperation> const& paths) override;

	virtual std::string GetName() const override;
private:
	static void initCOM();
};

