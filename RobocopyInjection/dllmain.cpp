#include "pch.h"
#pragma comment(lib, "Dbghelp.lib")

// Configuration: The Language ID we want to force (e.g., en-US)
constexpr static auto TargetLangId = 0x0409;

// Define the function signatures we are hooking
using GetUserDefaultLanguageFunc = LANGID(WINAPI*)(void);
using SetThreadUILanguageFunc = LANGID(WINAPI*)(LANGID);
using GetThreadUILanguageFunc = LANGID(WINAPI*)(void);

// Pointer to store the original address (optional for IAT, but good practice)
GetUserDefaultLanguageFunc pOriginalGetUserDefaultUILanguage{};
SetThreadUILanguageFunc pOriginalSetThreadUILanguage{};
GetThreadUILanguageFunc pOriginalGetThreadUILanguage{};

// ---------------------------------------------------------
// 1. The Fake Function
// ---------------------------------------------------------
LANGID WINAPI Fake_GetUserDefaultUILanguage(void) 
{
    return TargetLangId;
}

LANGID WINAPI Fake_SetThreadUILanguage(LANGID LangId) 
{
    return TargetLangId;
}

LANGID WINAPI Fake_GetThreadUILanguage(void) 
{
    return TargetLangId;
}

// ---------------------------------------------------------
// 2. The IAT Hooking Logic
// ---------------------------------------------------------
void InstallIATHook(const char* dllName, const char* funcName, void* newFuncAddress, void** ppOriginalAddress) {
    // Get the base address of the current process (the target EXE)
    HMODULE hModule = GetModuleHandle(NULL);

    ULONG size;
    // Locate the Import Directory in the PE Headers
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
        hModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);

    if (!pImportDesc) return; // No imports found

    // Loop through all imported DLLs to find the one we want (e.g., "kernel32.dll")
    while (pImportDesc->Name) {
        const char* moduleName = (char*)((BYTE*)hModule + pImportDesc->Name);

        // Check if the module name matches (case insensitive)
        if (_stricmp(moduleName, dllName) == 0) {
            // Found the DLL. Now loop through its functions.

            // OriginalFirstThunk points to the function NAMES
            PIMAGE_THUNK_DATA pThunkName = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->OriginalFirstThunk);
            // FirstThunk points to the function ADDRESSES (The IAT)
            PIMAGE_THUNK_DATA pThunkAddr = (PIMAGE_THUNK_DATA)((BYTE*)hModule + pImportDesc->FirstThunk);

            // Some compilers might not produce OriginalFirstThunk. If so, use FirstThunk (which points to names before binding)
            if (!pThunkName) {
                pThunkName = pThunkAddr;
            }

            while (pThunkName && pThunkName->u1.Function) {
                // Check if imported by Name (not by Ordinal)
                if (!(pThunkName->u1.Ordinal & IMAGE_ORDINAL_FLAG)) {
                    PIMAGE_IMPORT_BY_NAME pImportName = (PIMAGE_IMPORT_BY_NAME)((BYTE*)hModule + pThunkName->u1.AddressOfData);

                    // Check if this is the function we are looking for
                    if (strcmp(pImportName->Name, funcName) == 0) {

                        // FOUND IT! Now we overwrite the address.

                        // 1. Make the memory writable (IAT is usually Read-Only)
                        DWORD oldProtect;
                        VirtualProtect(&pThunkAddr->u1.Function, sizeof(uintptr_t), PAGE_READWRITE, &oldProtect);

                        // 2. Save original (optional)
                        if (ppOriginalAddress && *ppOriginalAddress == NULL) {
                            *ppOriginalAddress = (void*)pThunkAddr->u1.Function;
                        }

                        // 3. Overwrite with our function address
                        pThunkAddr->u1.Function = (uintptr_t)newFuncAddress;

                        // 4. Restore memory protections
                        VirtualProtect(&pThunkAddr->u1.Function, sizeof(uintptr_t), oldProtect, &oldProtect);
                        
                        return; // Hook installed, we are done.
                    }
                }
                pThunkName++;
                pThunkAddr++;
            }
        }
        pImportDesc++;
    }
}

// ---------------------------------------------------------
// 3. Entry Point
// ---------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // Force thread language immediately using the actual API (if available)
        HMODULE hKernel = GetModuleHandleA("kernel32.dll");
        if (hKernel) {
             SetThreadUILanguageFunc pSet = (SetThreadUILanguageFunc)GetProcAddress(hKernel, "SetThreadUILanguage");
             if (pSet) {
                 pSet(TargetLangId);
             }
        }

        // Hook GetUserDefaultUILanguage inside kernel32.dll
        InstallIATHook("kernel32.dll", "GetUserDefaultUILanguage", Fake_GetUserDefaultUILanguage, (void**)&pOriginalGetUserDefaultUILanguage);
        InstallIATHook("kernelbase.dll", "GetUserDefaultUILanguage", Fake_GetUserDefaultUILanguage, NULL);

        // Often apps call GetSystemDefaultUILanguage as fallback, hook that too
        InstallIATHook("kernel32.dll", "GetSystemDefaultUILanguage", Fake_GetUserDefaultUILanguage, NULL);
        InstallIATHook("kernelbase.dll", "GetSystemDefaultUILanguage", Fake_GetUserDefaultUILanguage, NULL);

        // Hook SetThreadUILanguage to force en-US
        InstallIATHook("kernel32.dll", "SetThreadUILanguage", Fake_SetThreadUILanguage, (void**)&pOriginalSetThreadUILanguage);
        InstallIATHook("kernelbase.dll", "SetThreadUILanguage", Fake_SetThreadUILanguage, NULL);

        // Hook GetThreadUILanguage
        InstallIATHook("kernel32.dll", "GetThreadUILanguage", Fake_GetThreadUILanguage, (void**)&pOriginalGetThreadUILanguage);
        InstallIATHook("kernelbase.dll", "GetThreadUILanguage", Fake_GetThreadUILanguage, NULL);
    }
    return TRUE;
}

