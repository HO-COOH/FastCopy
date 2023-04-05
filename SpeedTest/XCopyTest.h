#pragma once
#include "ITestBase.h"
#include "AutoRegister.h"

/**
 * This class calls Windows' built-in xcopy.exe
 */
class XCopyTest : public ITestBase, private AutoRegister<XCopyTest>
{
public:
	virtual bool Run(std::vector<TestOperation> const& paths) override;
	virtual std::string GetName() const override;
};

