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
#include "CopyOperationNames.h"
#include "Registry.h"


#pragma comment(lib,"runtimeobject")
#pragma comment(lib, "Shlwapi.lib")


void Print(IShellItemArray* items)
{
#ifdef DEBUG
    auto const count = ShellItemArray{ items }.size();
    OutputDebugString(std::to_wstring(count).data());
#endif
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
        Print(items);
        return SHStrDup(CommandRootTitle, name);
    }
    HRESULT GetIcon(IShellItemArray* items, PWSTR* icon) 
    { 
        Print(items);

        SHStrDup(std::format(L"{},{}", GetCurrentDllPath(), -IDI_ICON1).data(), icon);
        return S_OK;
    }
    HRESULT GetToolTip( IShellItemArray* items,  PWSTR* infoTip) 
    {
        Print(items);
        *infoTip = nullptr; return E_NOTIMPL; 
    }
    HRESULT GetCanonicalName(GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return E_NOTIMPL; }
    HRESULT GetState( IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState)
    {
        Print(selection);

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
        Print(selection);
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

#include <atlcomcli.h>  // for COM smart pointers
#include <atlbase.h>    // for COM smart pointers
#include <ShlObj_core.h>
#include <shlobj.h>

void ThrowIfFailed(...) {}
// Deleter for a PIDL allocated by the shell.
struct CoTaskMemDeleter
{
    void operator()(ITEMIDLIST* pidl) const { ::CoTaskMemFree(pidl); }
};
using UniquePidlPtr = std::unique_ptr< ITEMIDLIST, CoTaskMemDeleter >;
// Return value of GetCurrentExplorerFolders()
struct ExplorerFolderInfo
{
    HWND hwnd = nullptr;  // window handle of explorer
    UniquePidlPtr pidl;   // PIDL that points to current folder
};

// Get information about all currently open explorer windows.
// Throws std::system_error exception to report errors.
static std::vector< ExplorerFolderInfo > GetCurrentExplorerFolders()
{
    CComPtr< IShellWindows > pshWindows;
    ThrowIfFailed(
        pshWindows.CoCreateInstance(CLSID_ShellWindows),
        "Could not create instance of IShellWindows");

    long count = 0;
    ThrowIfFailed(
        pshWindows->get_Count(&count),
        "Could not get number of shell windows");

    std::vector< ExplorerFolderInfo > result;
    result.reserve(count);

    for (long i = 0; i < count; ++i)
    {
        ExplorerFolderInfo info;

        CComVariant vi{ i };
        CComPtr< IDispatch > pDisp;
        ThrowIfFailed(
            pshWindows->Item(vi, &pDisp),
            "Could not get item from IShellWindows");

        if (!pDisp)
            // Skip - this shell window was registered with a NULL IDispatch
            continue;

        CComQIPtr< IWebBrowserApp > pApp{ pDisp };
        if (!pApp)
            // This window doesn't implement IWebBrowserApp 
            continue;

        // Get the window handle.
        pApp->get_HWND(reinterpret_cast<SHANDLE_PTR*>(&info.hwnd));

        CComQIPtr< IServiceProvider > psp{ pApp };
        if (!psp)
            // This window doesn't implement IServiceProvider
            continue;

        CComPtr< IShellBrowser > pBrowser;
        if (FAILED(psp->QueryService(SID_STopLevelBrowser, &pBrowser)))
            // This window doesn't provide IShellBrowser
            continue;

        CComPtr< IShellView > pShellView;
        if (FAILED(pBrowser->QueryActiveShellView(&pShellView)))
            // For some reason there is no active shell view
            continue;

        CComQIPtr< IFolderView > pFolderView{ pShellView };
        if (!pFolderView)
            // The shell view doesn't implement IFolderView
            continue;

        // Get the interface from which we can finally query the PIDL of
        // the current folder.
        CComPtr< IPersistFolder2 > pFolder;
        if (FAILED(pFolderView->GetFolder(IID_IPersistFolder2, (void**)&pFolder)))
            continue;

        LPITEMIDLIST pidl = nullptr;
        if (SUCCEEDED(pFolder->GetCurFolder(&pidl)))
        {
            // Take ownership of the PIDL via std::unique_ptr.
            info.pidl = UniquePidlPtr{ pidl };
            result.push_back(std::move(info));
        }
    }

    return result;
}

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
        std::wstring argTransform{arg};
        std::transform(argTransform.begin(), argTransform.end(), argTransform.begin(), [](wchar_t c) { return c == L'\\' ? L'/' : c; });
        auto result = ShellExecute(
            NULL,
            L"open",
            std::format(LR"(fastcopy://"{}"|{})", argTransform, Registry{}.read(L"record")).data(),
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
        Print(selection);

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
        Print(items);
        SHStrDup(getName().data(), name);
        return S_OK;
    }
    HRESULT GetIcon(IShellItemArray* items,  PWSTR* icon) 
    { 
        Print(items);
        SHStrDup(std::format(
            L"{},{}", 
            GetCurrentDllPath(), 
            -IconProvider::GetForCurrentTheme().GetForOperation(m_op)).data(), 
            icon
        );
        return S_OK; 
    }
    HRESULT GetToolTip(IShellItemArray* items,  PWSTR* infoTip) 
    {
        Print(items);
        *infoTip = nullptr; 
        return E_NOTIMPL; 
    }
    HRESULT GetCanonicalName(GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return S_OK; }
    HRESULT GetState(IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        Print(selection);

        auto const state = State(selection);
        if (m_op == CopyOperation::Delete)
            *cmdState = ECS_DISABLED;
        else
            *cmdState = state == ECS_DISABLED ? ECS_HIDDEN : ECS_ENABLED;
        return S_OK;
    }


    HRESULT Invoke( IShellItemArray* selection,  IBindCtx* ctx) noexcept
    {
        /*
            if no files are selected, selection contains 1 element to the current invoked folder (Windows 11 only)
            On Windows 10, `selection` is `nullptr`
        */
        
        auto hwnd = GetForegroundWindow();
        switch (m_op)
        {
            case CopyOperation::Copy:
            case CopyOperation::Move:   return recordFilesImpl(selection);
            case CopyOperation::Paste:
            {

                for (auto const& info : GetCurrentExplorerFolders())
                {
                    if (info.hwnd == hwnd)
                    {
                        TCHAR* ptr;
                        SHGetNameFromIDList(info.pidl.get(), SIGDN_FILESYSPATH, &ptr);
                        return callMainProgramImpl(ptr);
                    }
                }

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
    DWORD const count = ShellItemArray{ items }.size();
    Print(items);
    if (!m_hasInit)
    {
        if (count == 0 && hasInvokedCopyOrCut())
        {
            auto pasteCommand = Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste);
            if (ptrs.empty())
            {
                ptrs.push_back(pasteCommand);
            }
            else
            {
                ptrs.insert(ptrs.begin() + 2, pasteCommand);
            }
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
        return subcommand->State(nullptr) == ECS_ENABLED;
    }))
    {
        ptrs.emplace_back(Microsoft::WRL::Make<SubCommand>(CopyOperation::Paste));
        ptrs.back()->SetParentForPasteForWindows10(ShellItemArray{ items }.front().GetPtr());
    }
}
