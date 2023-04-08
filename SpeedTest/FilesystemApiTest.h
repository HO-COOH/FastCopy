#pragma once
#include "ITestBase.h"
#include "AutoRegister.h"
class FilesystemApiTest : public ITestBase, private AutoRegister<FilesystemApiTest>
{
public:
	// Inherited via ITestBase
	virtual bool Run(std::vector<TestOperation> const& paths) override;
	virtual std::string GetName() const override;
};

