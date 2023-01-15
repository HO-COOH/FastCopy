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

#include "SharedMemoryItemDef.h"

#pragma comment(lib,"runtimeobject")
#pragma comment(lib, "Shlwapi.lib")

using namespace Microsoft::WRL;

class SubCommand;


void CheckResult(HRESULT hr, LPCWSTR prompt = nullptr)
{
    if (FAILED(hr) && prompt)
    {
        MessageBox(NULL, prompt, L"", 0);
    }
}

void AddItemToSharedMemory(IShellItem* item, std::vector<Path, boost::interprocess::allocator<Path, boost::interprocess::managed_shared_memory::segment_manager>>* v)
{
    wchar_t* name{};
    SFGAOF attrib{};
    CheckResult(item->GetDisplayName(SIGDN_FILESYSPATH, &name), L"GetDisplayName");
    CheckResult(item->GetAttributes(SFGAO_FOLDER, &attrib));
#ifdef ShowMessageBox
    MessageBox(NULL, L"Item", name, 0);
#endif

    Path path;
    path.content = name;
    path.type = (attrib & SFGAO_FOLDER ? Path::Type::Folder : Path::Type::File);
    v->push_back(path);
}

extern "C"
{
    __declspec(dllexport) void InvokeImpl(IShellItemArray* selection)
    {
        struct shm_remove
        {
            shm_remove() { boost::interprocess::shared_memory_object::remove("FastCopySharedMem"); }
        } remover;



        DWORD count{};
        CheckResult(selection->GetCount(&count), L"GetCount");

        using MyVector = std::vector<Path, boost::interprocess::allocator<Path, boost::interprocess::managed_shared_memory::segment_manager>>;

#ifdef ShowMessageBox
        MessageBox(NULL, L"Before make memory", L"", 0);
#endif
        boost::interprocess::managed_shared_memory sharedMemory{ boost::interprocess::create_only, "FastCopySharedMem", 65536 };

#ifdef ShowMessageBox
        MessageBox(NULL, L"After make memory", L"", 0);
#endif
        auto alloc = sharedMemory.get_segment_manager();
        auto v = sharedMemory.construct<MyVector>("MyVector")(alloc);
#ifdef ShowMessageBox
        MessageBox(NULL, L"After construct", L"", 0);
#endif

        for (auto i = 0; i < count; ++i)
        {
            IShellItem* item;
            CheckResult(selection->GetItemAt(i, &item), L"GetItemAt");
            AddItemToSharedMemory(item, v);
        }
    }


}

class __declspec(uuid("3282E233-C5D3-4533-9B25-44B8AAAFACFA")) TestExplorerCommandRoot : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand 
#ifdef EnableSubCommand
    , IEnumExplorerCommand
#endif
>
{
    std::vector<Microsoft::WRL::ComPtr<SubCommand>> ptrs
    { 
        Make<SubCommand>(L"赢"),
        Make<SubCommand>(L"麻"),
        Make<SubCommand>(L"了"),
    };

    decltype(ptrs.begin()) current;

    constexpr static auto inline CommandRootTitle = L"FastCopy";
public:

    // IExplorerCommand
    HRESULT GetTitle( IShellItemArray* items,  PWSTR* name)
    {
        return SHStrDup(CommandRootTitle, name);
    }
    HRESULT GetIcon( IShellItemArray*,  PWSTR* icon) { *icon = nullptr; return E_NOTIMPL; }
    HRESULT GetToolTip( IShellItemArray*,  PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
    HRESULT GetCanonicalName(GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return E_NOTIMPL; }
    HRESULT GetState( IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState)
    {
        *cmdState = ECS_ENABLED;
        return S_OK;
    }
    
    HRESULT GetFlags(_Out_ EXPCMDFLAGS* flags) 
    { 
#ifdef EnableSubCommand
        *flags = ECF_HASSUBCOMMANDS;
#else
        *flags = ECF_DEFAULT;
#endif
        return S_OK; 
    }
    
    HRESULT Invoke( IShellItemArray* selection,  IBindCtx*)
    {
#ifdef EnableSubCommand
        return E_NOTIMPL;
#endif
        DWORD count{};
        selection->GetCount(&count);

        if (count == 0)
            std::system("start fastcopy://b"); //direct start, use clipboard

        MessageBox(NULL, L"Entry", L"", 0);
        InvokeImpl(selection);
        std::system("start fastcopy://a");
        return S_OK;
    }

    HRESULT EnumSubCommands(IEnumExplorerCommand** enumCommands) 
    { 
#ifdef EnableSubCommand
        current = ptrs.begin();
        AddRef(); 
        auto hr = QueryInterface(IID_PPV_ARGS(enumCommands));
        return S_OK; 
#endif
        *enumCommands = nullptr;
        return E_NOTIMPL;
    }


#ifdef EnableSubCommand


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

    HRESULT  Skip(
        _In_ ULONG celt)
    {
        UNREFERENCED_PARAMETER(celt);
        return E_NOTIMPL;
    }

    HRESULT  Reset()
    {
        current = ptrs.begin();
        return S_OK;
    }

    HRESULT Clone(
        _Out_ IEnumExplorerCommand** ppenum)
    {
        *ppenum = nullptr;
        return E_NOTIMPL;
    }

#endif // EnableSubCommand
};




class  SubCommand final : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand>
{
    std::wstring m_title;
public:
    SubCommand(std::wstring title) : m_title{ title } {}
    virtual const EXPCMDSTATE State( IShellItemArray* selection) { return ECS_ENABLED; }

    // IExplorerCommand
    HRESULT GetTitle( IShellItemArray* items,  PWSTR* name)
    {
        SHStrDup(m_title.data(), name);
        return S_OK;
    }
    HRESULT GetIcon( IShellItemArray*,  PWSTR* icon) { *icon = nullptr; return E_NOTIMPL; }
    HRESULT GetToolTip( IShellItemArray*,  PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
    HRESULT GetCanonicalName(GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return S_OK; }
    HRESULT GetState( IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        *cmdState = State(selection);
        return S_OK;
    }
    HRESULT Invoke( IShellItemArray* selection,  IBindCtx*) noexcept
    {
        HWND parent = nullptr;

        std::wostringstream title;
        title << m_title;

        if (selection)
        {
            DWORD count;
            RETURN_IF_FAILED(selection->GetCount(&count));
            title << L" (" << count << L" selected items)";
        }
        else
        {
            title << L"(no selected items)";
        }

        MessageBox(parent, title.str().c_str(), L"TestCommand", MB_OK);
        return S_OK;
    }

    HRESULT GetFlags(EXPCMDFLAGS* flags) { *flags = ECF_DEFAULT; return S_OK; }
    HRESULT EnumSubCommands(IEnumExplorerCommand** enumCommands) { *enumCommands = nullptr; return E_NOTIMPL; }
};

CoCreatableClass(TestExplorerCommandRoot)
CoCreatableClassWrlCreatorMapInclude(TestExplorerCommandRoot)

STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
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
