#pragma once
#include <Windows.h>
#include <atomic>

class HostShutdownWatcher
{
public:
    using CleanupCallback = void(*)();

    static void Initialize(CleanupCallback cb);
    static void OnCoUninitialize();
    static void OnDllCanUnloadNow();
    static void OnTerminateProcess(HANDLE hProcess, UINT uExitCode);

private:
    enum class State {
        Running,
        AfterCoUninit,
        AfterCanUnload,
    };

    static std::atomic<State> s_state;
    static CleanupCallback s_callback;
    static DWORD s_ownerThreadId;
};
