#include "HostShutdownWatcher.h"

std::atomic<HostShutdownWatcher::State> HostShutdownWatcher::s_state{
    HostShutdownWatcher::State::Running
};
HostShutdownWatcher::CleanupCallback HostShutdownWatcher::s_callback = nullptr;
DWORD HostShutdownWatcher::s_ownerThreadId = 0;

void HostShutdownWatcher::Initialize(CleanupCallback cb)
{
    s_callback = cb;

    /*
    0:000> k
         # Child-SP          RetAddr               Call Site
        00 00000043`9a98f258 00007fff`a7246725     KERNELBASE!wil::details::DebugBreak+0x2
        01 00000043`9a98f260 00007fff`a728d2fd     FastCopyShellExtension!FastCopy::Settings::CommonSharedSettings::Shutdown+0xc5 [FastCopy\Shared\CommonSharedSettings.cpp @ 44] 
        02 00000043`9a98f380 00007fff`a729d6af     FastCopyShellExtension!FastCopyCleanupCallback+0x6d [FastCopy\FastCopyShellExtension\dllmain.cpp @ 68] 
        03 00000043`9a98f4a0 00007fff`a729d146     FastCopyShellExtension!HostShutdownWatcher::OnTerminateProcess+0x6f [FastCopy\FastCopyShellExtension\HostShutdownWatcher.cpp @ 36] 
        04 00000043`9a98f5e0 00007ff6`55c62085     FastCopyShellExtension!`anonymous namespace'::DetouredTerminateProcess+0x36 [FastCopy\FastCopyShellExtension\HostProcessHook.cpp @ 27] 
        05 00000043`9a98f6f0 00007ff6`55c61290     DllHost!wWinMain+0x191
        06 00000043`9a98f9b0 00007ff8`bec9259d     DllHost!__scrt_common_main_seh+0x110
        07 00000043`9a98f9f0 00007ff8`bf52af78     KERNEL32!BaseThreadInitThunk+0x1d
        08 00000043`9a98fa20 00000000`00000000     ntdll!RtlUserThreadStart+0x28
        0:000> ~#
        .  0  Id: 91c0.900c Suspend: 1 Teb: 00000043`9ab38000 Unfrozen
                Start: DllHost!wWinMainCRTStartup (00007ff6`55c61310)
                Priority: 0  Priority class: 32  Affinity: ffff
    */
    s_ownerThreadId = ::GetCurrentThreadId();  // Get the thread ID of the host process
}

void HostShutdownWatcher::OnCoUninitialize()
{
    if (s_ownerThreadId != 0 && ::GetCurrentThreadId() != s_ownerThreadId)
    {
        // Non-host thread: ignore this CoUninitialize
        return;
    }
    State expected = State::Running;
    s_state.compare_exchange_strong(expected, State::AfterCoUninit);
}

void HostShutdownWatcher::OnDllCanUnloadNow()
{
    State expected = State::AfterCoUninit;
    s_state.compare_exchange_strong(expected, State::AfterCanUnload);
}

void HostShutdownWatcher::OnTerminateProcess(HANDLE hProcess, UINT uExitCode)
{
    DWORD pid = GetProcessId(hProcess);
    if (pid == 0 || pid == GetCurrentProcessId())
    {
        State state = s_state.load(std::memory_order_acquire);
        if (state == State::AfterCoUninit)
        {
            if (s_callback)
            {
                __try {
                    s_callback();
                }
                __except (EXCEPTION_EXECUTE_HANDLER) {
                }
            }
        }
    }
}
