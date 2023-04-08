#pragma once
#include "ITestBase.h"
#include "AutoRegister.h"

/**
 * Calls windows' built-in robocopy.exe
 */
class RobocopyTest :
    public ITestBase,
    public AutoRegister<RobocopyTest>
{
public:
    // Inherited via ITestBase
    virtual bool Run(std::vector<TestOperation> const& paths) override;

    virtual std::string GetName() const override;
private:
    static const std::wstring ApplicationName;
};

