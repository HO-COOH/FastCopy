#include "pch.h"
#pragma comment(lib, "Dbghelp.lib")

constexpr auto TargetLanguage = L"en-US\0";

static bool ForceEnglish()
{
    ULONG numLanguages{};
	auto const processOK = SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, TargetLanguage, &numLanguages);

    SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, TargetLanguage, nullptr);
	return processOK && numLanguages > 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) 
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        ForceEnglish();

        /*
            With /UNICODE robocopy writes Unicode only to a real console(WriteConsoleW)
            To a pipe it converts its output to its console's output code page.
            Because we run it with CREATE_NO_WINDOW so it gets its own console, which starts with the system OEM code page (eg. 437, 936)
            characters missing from it become '?' and the rest are not UTF-8. 
            Switch that console to UTF-8, which is what FastCopy parses.
        */
        SetConsoleOutputCP(CP_UTF8);
    }
    return TRUE;
}
