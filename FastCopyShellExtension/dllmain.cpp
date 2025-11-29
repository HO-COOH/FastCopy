//// dllmain.cpp : Defines the entry point for the DLL application.
#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <Windows.h>
#include "FastCopyRootCommand.h"
#include "FastCopySubCommand.h"

#include "DebugPrint.h"

#pragma comment(lib,"runtimeobject")
#pragma comment(lib, "Shlwapi.lib")


CoCreatableClass(FastCopyRootCommand)
CoCreatableClassWrlCreatorMapInclude(FastCopyRootCommand)

STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Microsoft::WRL::Module<Microsoft::WRL::ModuleType::InProc>::GetModule()
        .GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
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
        auto& logger = FastCopyLogger::Instance();
        logger.LogProcessInfo(L"DLL_PROCESS_ATTACH");
        logger.LogDllPath(hModule, L"DLL_PROCESS_ATTACH");
        logger.SetBreakOnLog(true, /*minLevel=*/ FastCopyLogger::Verbosity::Trace);
    }
    break;
    case DLL_PROCESS_DETACH:
    {
        auto& logger = FastCopyLogger::Instance();
        logger.SetBreakOnLog(false);
        logger.LogProcessInfo(L"DLL_PROCESS_DETACH");
        logger.LogDllPath(hModule, L"DLL_PROCESS_DETACH");
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
