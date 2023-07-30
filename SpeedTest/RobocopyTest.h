#pragma once
#include "ICopyBase.h"
#include "AutoRegister.h"

/**
 * Calls windows' built-in robocopy.exe
 */
class RobocopyTest :
    public IImplementationBase, public AutoRegister<RobocopyTest>
{
public:
    // Inherited via ICopyBase
    virtual bool Run(std::vector<TestOperation> const& paths) override;

    virtual std::string GetName() const override;
private:
    static const std::wstring ApplicationName;
};

