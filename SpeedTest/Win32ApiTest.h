#pragma once
#include "ICopyBase.h"
#include "AutoRegister.h"
class Win32ApiTest :
    public IImplementationBase,
    public AutoRegister<Win32ApiTest>
{
public:
    // Inherited via ICopyBase
    virtual bool Run(std::vector<TestOperation> const& paths) override;
    virtual std::string GetName() const override;
};

