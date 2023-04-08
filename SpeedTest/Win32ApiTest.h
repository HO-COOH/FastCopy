#pragma once
#include "ITestBase.h"
#include "AutoRegister.h"
class Win32ApiTest :
    public ITestBase,
    public AutoRegister<Win32ApiTest>
{
public:
    // Inherited via ITestBase
    virtual bool Run(std::vector<TestOperation> const& paths) override;
    virtual std::string GetName() const override;
};

