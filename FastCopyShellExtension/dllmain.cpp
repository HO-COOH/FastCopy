//// dllmain.cpp : Defines the entry point for the DLL application.
#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <Windows.h>
#include "FastCopyRootCommand.h"
#include "FastCopySubCommand.h"

#include "DebugHelper.hpp"
#include "DebugPrint.h"
#include "HostShutdownWatcher.h"
#include "HostProcessHook.h"
#include "CommonSharedSettings.h"

#pragma comment(lib,"runtimeobject")
#pragma comment(lib, "Shlwapi.lib")


CoCreatableClass(FastCopyRootCommand)
CoCreatableClassWrlCreatorMapInclude(FastCopyRootCommand)

static HMODULE m_hCurrentModule = nullptr;

STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Microsoft::WRL::Module<Microsoft::WRL::ModuleType::InProc>::GetModule()
        .GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    FC_LOG_DEBUG(L"DllCanUnloadNow");

    // Tell the state machine: AfterCoUninit -> AfterCanUnload
    HostShutdownWatcher::OnDllCanUnloadNow();

    auto count = Microsoft::WRL::Module<Microsoft::WRL::InProc>::GetModule().GetObjectCount();
    if (count == 0) {
        FC_LOG_DEBUG(L"DllCanUnloadNow: count=0");
        return S_OK;
    }
    else {
        FC_LOG_DEBUG(L"DllCanUnloadNow: count={}.", count);
        return S_FALSE;
    }
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    FC_LOG_DEBUG(L"DllGetClassObject: CLSID={}.", Win32FormatGuid(rclsid).c_str());
    auto hr = Microsoft::WRL::Module<Microsoft::WRL::InProc>::GetModule()
        .GetClassObject(rclsid, riid, instance);
    FC_LOG_DEBUG(L"DllGetClassObject: hr={}", hr);
    return hr;
}

static void __stdcall FastCopyCleanupCallback()
{
    auto& logger = FastCopyLogger::Instance();

    logger.LogProcessInfo(L"FastCopyCleanupCallback (TerminateProcess path)");

    // We need to turn off any logging operations first before proceeding.
    logger.SetGlobalVerbosity(FastCopyLogger::Verbosity::Off);
    logger.SetEnabled(false);
    logger.SetBreakOnLog(false);

    // Shutdown Shared Settings, release resources
    ::FastCopy::Settings::CommonSharedSettings::Instance().Shutdown();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);

        m_hCurrentModule = hModule;

        // Initialize the state machine
        HostShutdownWatcher::Initialize(&FastCopyCleanupCallback);

        // Install detours hook
        HostProcessHook::Install();

        auto& logger = FastCopyLogger::Instance();
        logger.LogProcessInfo(L"DLL_PROCESS_ATTACH");
        logger.LogDllPath(hModule, L"DLL_PROCESS_ATTACH");

        DEBUG_WAIT_FOR_DEBUGGER(/* timeout= ms */60000);
    }
    break;
    case DLL_PROCESS_DETACH:
    {
        // NOTE:
        // Originally we suspected that dllhost.exe might skip CoFreeUnusedLibraries
        // for unused COM modules, which would mean:
        //   - DllCanUnloadNow is never called, and
        //   - the process could end directly without going through DllMain.
        //
        // The "expected" graceful sequence was something like:
        //   CoUninitialize
        //   -> RtlDllShutdownInProgress
        //   -> DllCanUnloadNow arrives
        //   -> check COM ref counts == 0
        //   -> DllCanUnloadNow returns S_OK
        //   -> FreeLibrary
        //   -> CallDllMain(DLL_PROCESS_DETACH)
        //
        // In practice this is NOT guaranteed. dllhost.exe can (and does) call
        // TerminateProcess on itself after CoUninitialize, which bypasses
        // DllMain(DLL_PROCESS_DETACH) entirely. This is consistent with the
        // documentation: if the process is terminating (lpReserved != NULL),
        // all other threads are already gone or have been terminated by
        // ExitProcess/TerminateProcess, and DLL detach notifications are not
        // guaranteed to run.
        //
        // To handle this reliably we now:
        //   - hook CoUninitialize and TerminateProcess via HostProcessHook
        //   - track the host state in HostShutdownWatcher
        //   - when CoUninitialize has been observed but DllCanUnloadNow has not
        //     been called afterwards, we invoke FastCopyCleanupCallback() from
        //     the TerminateProcess detour.
        //
        // FastCopyCleanupCallback() is responsible for shutting down our
        // internal resources (e.g. CommonSharedSettings monitor thread,
        // logging, etc.) on the "hard" TerminateProcess path.
        //
        // Therefore:
        //   - DLL_PROCESS_DETACH is only relied on for the normal FreeLibrary
        //     unload path;
        //   - the dllhost.exe TerminateProcess case is now handled explicitly
        //     by the hooks and does not depend on DllMain being called.
        //
        // For debugging you can still use the following breakpoints:
        //   > bp FastCopyShellExtension!DllCanUnloadNow
        //   > bp FastCopyShellExtension!DllMain
        //   > bp combase!CoUninitialize
        //   > bp ntdll!RtlDllShutdownInProgress
        //   > bp kernelbase!TerminateProcess

        // (No additional cleanup is required here; resource shutdown is handled
        // either by normal object lifetime or by FastCopyCleanupCallback().)
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
