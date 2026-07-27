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
    }
    return TRUE;
}
