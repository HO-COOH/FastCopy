//// dllmain.cpp : Defines the entry point for the DLL application.
#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <shobjidl_core.h>
#include <wil/resource.h>
#include <winrt/base.h>
#include <string>
#include <vector>
#include <sstream>
#include <Windows.h>
#include <Shlwapi.h>
#include "CopyOperation.h"
#include "Recorder.h"
#include "IconProvider.h"


#pragma comment(lib,"runtimeobject")
#pragma comment(lib, "Shlwapi.lib")



class SubCommand;

static std::wstring const& GetCurrentDllPath()
{
    static std::wstring ret = []
    {
        wchar_t path[MAX_PATH]{};
        GetModuleFileName(GetModuleHandle(L"FastCopyShellExtension.dll"), path, std::size(path));
        return std::wstring{ path };
    }();
    return ret;
}


static void CheckResult(HRESULT hr, LPCWSTR prompt = nullptr)
{
    if (FAILED(hr) && prompt)
    {
        MessageBox(NULL, prompt, L"", 0);
    }
}


class __declspec(uuid("3282E233-C5D3-4533-9B25-44B8AAAFACFA")) TestExplorerCommandRoot : 
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, 
        IExplorerCommand, 
        IEnumExplorerCommand>
{
    constexpr static auto inline CommandRootTitle = L"FastCopy";

    bool m_hasInit = false;
    std::vector<Microsoft::WRL::ComPtr<SubCommand>> ptrs;

    decltype(ptrs.begin()) current;

    static bool hasInvokedCopyOrCut()
    {
        return Recorder::HasRecord();
    }

    void init(IShellItemArray* items)
    {
        if (!m_hasInit)
        {
            if (hasInvokedCopyOrCut())
            {
                if (ptrs.empty())
                    ptrs.push_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste));
                else
                    ptrs.insert(ptrs.begin() + 2, Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste));
                m_hasInit = true;
            }
        }

        if (!items)
            return;

        DWORD count{};
        items->GetCount(&count);
        if (count != 0)
        {
            ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Copy));
            ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Move));
            ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Delete));
            m_hasInit = true;
        }

    }
public:

    TestExplorerCommandRoot() = default;

    #pragma region IExplorerCommand
    HRESULT GetTitle( IShellItemArray* items,  PWSTR* name)
    {

        return SHStrDup(CommandRootTitle, name);
    }
    HRESULT GetIcon(IShellItemArray* items, PWSTR* icon) 
    { 
        //MessageBoxW(NULL, path, L"", 0);
        SHStrDup(std::format(L"{},{}", GetCurrentDllPath(), -IDI_ICON1).data(), icon);
        return S_OK;
    }
    HRESULT GetToolTip( IShellItemArray*,  PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
    HRESULT GetCanonicalName(GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return E_NOTIMPL; }
    HRESULT GetState( IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState)
    {
        init(selection);
        if (m_hasInit && !ptrs.empty())
            *cmdState = ECS_ENABLED;
        else
            *cmdState = ECS_DISABLED;
        return S_OK;
    }
    
    HRESULT GetFlags(_Out_ EXPCMDFLAGS* flags) 
    { 
        *flags = ECF_HASSUBCOMMANDS;
        return S_OK; 
    }
    
    HRESULT Invoke( IShellItemArray* selection,  IBindCtx*)
    {
        return E_NOTIMPL;
    }

    HRESULT EnumSubCommands(IEnumExplorerCommand** enumCommands) 
    { 
        current = ptrs.begin();
        AddRef(); 
        auto hr = QueryInterface(IID_PPV_ARGS(enumCommands));
        return S_OK; 
    }
    #pragma endregion

    #pragma region IEnumExplorerCommand
    HRESULT Next(ULONG celt, IExplorerCommand** pUICommand, ULONG* pceltFetched) override
    {
        //MessageBox(NULL, L"Next called", L"", 0);
        ULONG Fetched = 0;

        for (
            ULONG i = 0;
            (i < celt) &&
            (this->current != this->ptrs.cend());
            ++i)
        {
            current->CopyTo(&pUICommand[0]);
            ++current;
            ++Fetched;
        }

        if (pceltFetched)
        {
            *pceltFetched = Fetched;
        }

        return (Fetched == celt) ? S_OK : S_FALSE;
    }

    HRESULT Skip(ULONG)
    {
        return E_NOTIMPL;
    }

    HRESULT Reset()
    {
        current = ptrs.begin();
        return S_OK;
    }

    HRESULT Clone(IEnumExplorerCommand** ppenum)
    {
        *ppenum = nullptr;
        return E_NOTIMPL;
    }
    #pragma endregion
};



class SubCommand final : 
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, 
        IExplorerCommand>
{
    CopyOperation m_op;

    auto& getName() const
    {
        auto const& nameConstants = CopyOperationNames::GetInstance();
        switch (m_op)
        {
            case CopyOperation::Copy:   return nameConstants.Copy;
            case CopyOperation::Move:   return nameConstants.Move;
            case CopyOperation::Delete: return nameConstants.Delete;
            case CopyOperation::Paste:  return nameConstants.Paste;
            default: assert(false);
        }
    }

    HRESULT recordFilesImpl(IShellItemArray* selection)
    {
        DWORD count;
        selection->GetCount(&count);
        Recorder recorder{ m_op };
        while (count--)
        {
            IShellItem* ptr;
            if (SUCCEEDED(selection->GetItemAt(count, &ptr)))
                recorder << *ptr;
        }
        return S_OK;
    }

    HRESULT callMainProgramImpl()
    {
        auto result = ShellExecute(
            NULL,
            L"open",
            L"fastcopy://",
            nullptr,
            nullptr,
            SW_SHOW
        );
        return S_OK;
    }
public:
    SubCommand(CopyOperation op) : m_op{op}
    {
    }

    virtual const EXPCMDSTATE State( IShellItemArray* selection) { return ECS_ENABLED; }

    // IExplorerCommand
    HRESULT GetTitle( IShellItemArray* items,  PWSTR* name)
    {
        SHStrDup(getName().data(), name);
        return S_OK;
    }
    HRESULT GetIcon(IShellItemArray*,  PWSTR* icon) 
    { 
        SHStrDup(std::format(
            L"{},{}", 
            GetCurrentDllPath(), 
            -IconProvider::GetForCurrentTheme().GetForOperation(m_op)).data(), 
            icon
        );
        return S_OK; 
    }
    HRESULT GetToolTip(IShellItemArray*,  PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
    HRESULT GetCanonicalName(GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return S_OK; }
    HRESULT GetState(IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        *cmdState = State(selection);
        return S_OK;
    }
    HRESULT Invoke( IShellItemArray* selection,  IBindCtx*) noexcept
    {
        switch (m_op)
        {
            case CopyOperation::Copy:
            case CopyOperation::Move:   return recordFilesImpl(selection);
            case CopyOperation::Paste:  return callMainProgramImpl();
            case CopyOperation::Delete:
                recordFilesImpl(selection);
                return callMainProgramImpl();
            default:
                assert(false);
        }
    }

    HRESULT GetFlags(EXPCMDFLAGS* flags) { *flags = ECF_DEFAULT; return S_OK; }
    HRESULT EnumSubCommands(IEnumExplorerCommand** enumCommands) { *enumCommands = nullptr; return E_NOTIMPL; }
};


#pragma region BoilerPlate
CoCreatableClass(TestExplorerCommandRoot)
CoCreatableClassWrlCreatorMapInclude(TestExplorerCommandRoot)

STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Microsoft::WRL::Module<Microsoft::WRL::ModuleType::InProc>::GetModule()
        .GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Microsoft::WRL::Module<Microsoft::WRL::InProc>::GetModule()
        .GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Microsoft::WRL::Module<Microsoft::WRL::InProc>::GetModule()
        .GetClassObject(rclsid, riid, instance);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
#pragma endregion
