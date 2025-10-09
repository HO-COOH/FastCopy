#include "pch.h"
#include "CommandLineHandler.h"

int __stdcall wWinMain(
    _In_ HINSTANCE, 
    _In_opt_ HINSTANCE, 
    _In_ PWSTR cmd, 
    _In_ int)
{
    CommandLineHandler{ cmd }.Run();
    return 0;
}