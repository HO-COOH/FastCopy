#pragma once
#include "ICopyBase.h"
#include "AutoRegister.h"
class FilesystemApiTest : public IImplementationBase, private AutoRegister<FilesystemApiTest>
{
public:
	// Inherited via ICopyBase
	virtual bool Run(std::vector<TestOperation> const& paths) override;
	virtual std::string GetName() const override;
};

