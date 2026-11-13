#include "pch.h"
#include "../Public/RobocopyInjectionHandshake.h"

#pragma comment(lib, "Dbghelp.lib")


using WfopenFunc = FILE* (__cdecl*)(wchar_t const*, wchar_t const*);
static WfopenFunc pOriginalWfopen{};

static FILE* __cdecl Hooked_wfopen(wchar_t const* filename, wchar_t const* mode)
{
    if (!filename || !mode || std::wstring_view{ mode } != L"r")
        return pOriginalWfopen(filename, mode);

    /*
       robocopy.exe links against legacy msvcrt.dll and opens .rcj job files with _wfopen(path, L"r")
       Switching read-mode .rcj opens to "r, ccs=UTF-8" makes msvcrt's fgetws decode UTF-8 directly. 
    */
    if (std::wstring_view const filenameView{ filename }; filenameView.ends_with(L".RCJ") || filenameView.ends_with(L".rcj"))
        return pOriginalWfopen(filename, L"r, ccs=UTF-8");

    return pOriginalWfopen(filename, mode);
}

static bool PatchIATEntry(ULONG_PTR& iatSlot)
{
    DWORD oldProtect{};
    if (!VirtualProtect(&iatSlot, sizeof(iatSlot), PAGE_READWRITE, &oldProtect))
        return false;

    if (!pOriginalWfopen)
        pOriginalWfopen = reinterpret_cast<WfopenFunc>(iatSlot);

    iatSlot = reinterpret_cast<ULONG_PTR>(&Hooked_wfopen);
    VirtualProtect(&iatSlot, sizeof(iatSlot), oldProtect, &oldProtect);
    return true;
}

static bool InstallWfopenIATHook()
{
    HMODULE hModule = GetModuleHandleW(nullptr);
    if (!hModule)
        return false;

    ULONG size{};
    auto* pImportDesc = static_cast<PIMAGE_IMPORT_DESCRIPTOR>(ImageDirectoryEntryToData(
        hModule, 
		TRUE,                           //map as image
        IMAGE_DIRECTORY_ENTRY_IMPORT,   //Import directory
        &size
    ));

    if (!pImportDesc)
        return false;

    //In every valid entry, Name is guaranteed to be non-zero
    for (auto* base = reinterpret_cast<BYTE*>(hModule); pImportDesc->Name; ++pImportDesc)
    {
        for (
            auto *pThunkName = reinterpret_cast<PIMAGE_THUNK_DATA>(base + (pImportDesc->OriginalFirstThunk ? pImportDesc->OriginalFirstThunk : pImportDesc->FirstThunk)),
                 *pThunkAddr = reinterpret_cast<PIMAGE_THUNK_DATA>(base + pImportDesc->FirstThunk);
            pThunkName && pThunkName->u1.Function;
            ++pThunkName, ++pThunkAddr)
        {
            if (IMAGE_SNAP_BY_ORDINAL(pThunkName->u1.Ordinal))
                continue;

            if (std::string_view{ reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(base + pThunkName->u1.AddressOfData)->Name } != "_wfopen")
                continue;

            return PatchIATEntry(pThunkAddr->u1.Function);
        }
    }

    return false;
}

static bool ForceEnglish()
{
    constexpr auto TargetLanguage = L"en-US\0";

    ULONG numLanguages{};
    return SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, TargetLanguage, &numLanguages) &&
        numLanguages > 0 &&
        SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME, TargetLanguage, nullptr);
}

static bool WriteHandshakeReady()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!hOut || hOut == INVALID_HANDLE_VALUE)
        return false;

    DWORD written{};
    return WriteFile(
        hOut, 
        RobocopyInjectionHandshake::ReadyLine.data(),
        static_cast<DWORD>(RobocopyInjectionHandshake::ReadyLine.size()),
        &written, 
        nullptr
    ) && written == RobocopyInjectionHandshake::ReadyLine.size();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) 
{
    if (ul_reason_for_call != DLL_PROCESS_ATTACH)
        return TRUE;

    DisableThreadLibraryCalls(hModule);

    if (!ForceEnglish())
        TerminateProcess(GetCurrentProcess(), static_cast<UINT>(RobocopyInjectionHandshake::InjectionStatus::ForceEnglishFailed));

    if (!InstallWfopenIATHook())
        TerminateProcess(GetCurrentProcess(), static_cast<UINT>(RobocopyInjectionHandshake::InjectionStatus::HookWfopenFailed));

    /*
        With /UNICODE robocopy writes Unicode only to a real console(WriteConsoleW)
        To a pipe it converts its output to its console's output code page.
        Because we run it with CREATE_NO_WINDOW so it gets its own console, which starts with the system OEM code page (eg. 437, 936)
        characters missing from it become '?' and the rest are not UTF-8. 
        Switch that console to UTF-8, which is what FastCopy parses.
    */
    if (!SetConsoleOutputCP(CP_UTF8) || !WriteHandshakeReady())
        TerminateProcess(GetCurrentProcess(), static_cast<UINT>(RobocopyInjectionHandshake::InjectionStatus::SetConsoleUtf8Failed));

    return TRUE;
}
