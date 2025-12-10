#include "HostProcessHook.h"
#include "HostShutdownWatcher.h"
#include <Windows.h>
#include <detours/detours.h>


namespace
{
    using PFN_TerminateProcess = BOOL(WINAPI*)(HANDLE, UINT);
    using PFN_CoUninitialize = void (WINAPI*)(void);

    PFN_TerminateProcess g_RealTerminateProcess = ::TerminateProcess;
    PFN_CoUninitialize   g_RealCoUninitialize = ::CoUninitialize;

    bool g_HooksInstalled = false;

    void WINAPI DetouredCoUninitialize()
    {
        HostShutdownWatcher::OnCoUninitialize();
        g_RealCoUninitialize();
    }

    BOOL WINAPI DetouredTerminateProcess(HANDLE hProcess, UINT uExitCode)
    {
        HostShutdownWatcher::OnTerminateProcess(hProcess, uExitCode);
        return g_RealTerminateProcess(hProcess, uExitCode);
    }
}

bool HostProcessHook::Install()
{
    if (g_HooksInstalled)
        return true;

    LONG error = NO_ERROR;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    error = DetourAttach(
        reinterpret_cast<PVOID*>(&g_RealCoUninitialize),
        reinterpret_cast<PVOID>(DetouredCoUninitialize)
    );
    if (error != NO_ERROR)
    {
        DetourTransactionAbort();
        return false;
    }

    error = DetourAttach(
        reinterpret_cast<PVOID*>(&g_RealTerminateProcess),
        reinterpret_cast<PVOID>(DetouredTerminateProcess)
    );
    if (error != NO_ERROR)
    {
        DetourTransactionAbort();
        return false;
    }

    error = DetourTransactionCommit();
    if (error == NO_ERROR)
    {
        g_HooksInstalled = true;
        return true;
    }
    else
    {
        DetourTransactionAbort();
        return false;
    }
}

void HostProcessHook::Uninstall()
{
    if (!g_HooksInstalled)
        return;

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach(
        reinterpret_cast<PVOID*>(&g_RealCoUninitialize),
        reinterpret_cast<PVOID>(DetouredCoUninitialize)
    );
    DetourDetach(
        reinterpret_cast<PVOID*>(&g_RealTerminateProcess),
        reinterpret_cast<PVOID>(DetouredTerminateProcess)
    );

    DetourTransactionCommit();

    g_HooksInstalled = false;
}
