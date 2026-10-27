#include "pch.h"
#include "CommandLineHandler.h"
#include <clocale>

int __stdcall wWinMain(
    _In_ HINSTANCE, 
    _In_opt_ HINSTANCE, 
    _In_ PWSTR cmd, 
    _In_ int)
{
    /*
        In the default "C" locale std::wcout fails permanently on the first character above U + 00FF(eg.a non-ASCII user name in a temp path)
        Making the diagnostic console stop printing anything 
    */
    setlocale(LC_CTYPE, ".UTF-8");
    CommandLineHandler{ cmd }.Run();
    return 0;
}