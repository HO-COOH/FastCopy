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
#include "ShellItemArray.h"
#include "ShellItem.h"


#pragma comment(lib,"runtimeobject")
#pragma comment(lib, "Shlwapi.lib")


void Print(IShellItemArray* items)
{
    if (!items)
        return;

    if (ShellItemArray{ items }.size() == 0)
        return;

    for (auto item : ShellItemArray{ items })
    {
        OutputDebugString(item.GetDisplayName());
        OutputDebugString(L"\n");
    }
}

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

    void init(IShellItemArray* items);
public:

    TestExplorerCommandRoot() = default;

    #pragma region IExplorerCommand
    HRESULT GetTitle( IShellItemArray* items,  PWSTR* name)
    {
        return SHStrDup(CommandRootTitle, name);
    }
    HRESULT GetIcon(IShellItemArray* items, PWSTR* icon) 
    { 
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
    HRESULT recordFilesImpl(IShellItemArray* selection)
    {
        Recorder recorder{ m_op };
        for (auto item : ShellItemArray{ selection })
        {
            if (item.GetPtr())
                recorder << item;
        }
        return S_OK;
    }

    HRESULT callMainProgramImpl(std::wstring_view arg)
    {
        auto result = ShellExecute(
            NULL,
            L"open",
            std::format(LR"(fastcopy://"{}")", arg).data(),
            nullptr,
            nullptr,
            SW_SHOW
        );
        return S_OK;
    }

    IShellItem* m_parent{};
public:
    SubCommand(CopyOperation op) : m_op{op}
    {
    }

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

    void SetParentForPasteForWindows10(IShellItem* parent)
    {
        m_parent = parent;
    }

    virtual const EXPCMDSTATE State( IShellItemArray* selection) 
    { 
        switch (m_op)
        {
            case CopyOperation::Copy:
            case CopyOperation::Move:
                return ShellItemArray{ selection }.size() != 0 ? ECS_ENABLED : ECS_DISABLED;
            case CopyOperation::Paste:
                return (Recorder::HasRecord() && (!selection || ShellItemArray{ selection }.size() == 0)) ? ECS_ENABLED : ECS_DISABLED;
            case CopyOperation::Delete:
                return ECS_DISABLED;
            default:
                return ECS_ENABLED;
        }
    }

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
        auto const state = State(selection);
        if (m_op == CopyOperation::Delete)
            *cmdState = ECS_DISABLED;
        else
            *cmdState = state == ECS_DISABLED ? ECS_HIDDEN : ECS_ENABLED;
        return S_OK;
    }
    HRESULT Invoke( IShellItemArray* selection,  IBindCtx*) noexcept
    {
        /*
            if no files are selected, selection contains 1 element to the current invoked folder
        */
        switch (m_op)
        {
            case CopyOperation::Copy:
            case CopyOperation::Move:   return recordFilesImpl(selection);
            case CopyOperation::Paste:
            {
                ShellItem psi{ selection ? ShellItemArray{ selection }[0] : m_parent };
                return callMainProgramImpl(psi.GetDisplayName());
            }
            case CopyOperation::Delete:
                recordFilesImpl(selection);
                return callMainProgramImpl(L"");
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

void TestExplorerCommandRoot::init(IShellItemArray* items)
{
    DWORD const count = items ? ShellItemArray{ items }.size() : 0;

    //MessageBox(NULL, std::to_wstring(count).data(), L"", 0);
    if (!m_hasInit)
    {
        if (count == 0 && hasInvokedCopyOrCut())
        {
            if (ptrs.empty())
                ptrs.push_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste));
            else
                ptrs.insert(ptrs.begin() + 2, Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste));
            m_hasInit = true;
        }
    }



    if (count != 0)
    {
        ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Copy));
        ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Move));
        ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Delete));
        m_hasInit = true;
    }

    if (!ptrs.empty() && std::none_of(ptrs.begin(), ptrs.end(), [items](Microsoft::WRL::ComPtr<SubCommand> const& subcommand) {
        OutputDebugString(subcommand->getName().data());
        OutputDebugString(std::to_wstring(subcommand->State(nullptr) == ECS_ENABLED).data());
        OutputDebugString(L"\n");
        return subcommand->State(nullptr) == ECS_ENABLED;
        }))
    {
        ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste));
        ptrs.back()->SetParentForPasteForWindows10(ShellItemArray{ items }.front().GetPtr());
    }
}
