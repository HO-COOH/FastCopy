#pragma once
#include <wrl/implements.h>
#include <ShObjIdl_core.h>
#include <boost/container/static_vector.hpp>

#include "DebugPrint.h"

class FastCopySubCommand;

class __declspec(uuid("3282E233-C5D3-4533-9B25-44B8AAAFACFA")) FastCopyRootCommand :
    public Microsoft::WRL::RuntimeClass
    <
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        IExplorerCommand,
        IEnumExplorerCommand,
        IObjectWithSite
    >
{
    bool m_hasInit = false;
    boost::container::static_vector<Microsoft::WRL::ComPtr<FastCopySubCommand>, 4> m_subCommands{};
    Microsoft::WRL::ComPtr<IUnknown> m_site;

    decltype(m_subCommands.begin()) m_subCommandIter;

    static bool hasInvokedCopyOrCut();
    void init(IShellItemArray* items);
public:

    FastCopyRootCommand() = default;

    // IObjectWithSite
    IFACEMETHODIMP SetSite(IUnknown* pUnkSite) override;
    IFACEMETHODIMP GetSite(REFIID riid, void** ppvSite) override;

#pragma region IExplorerCommand
    HRESULT GetTitle(IShellItemArray* items, PWSTR* name) override;
    HRESULT GetIcon(IShellItemArray* items, PWSTR* icon) override;
    HRESULT GetToolTip(IShellItemArray* items, PWSTR* infoTip) override;
    HRESULT GetCanonicalName(GUID* guidCommandName) override;
    HRESULT GetState(IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState) override;
    HRESULT GetFlags(EXPCMDFLAGS* flags) override;
    HRESULT Invoke(IShellItemArray* selection, IBindCtx*) override;
    HRESULT EnumSubCommands(IEnumExplorerCommand** enumCommands) override;
#pragma endregion

#pragma region IEnumExplorerCommand
    HRESULT Next(ULONG celt, IExplorerCommand** pUICommand, ULONG* pceltFetched) override;
    HRESULT Skip(ULONG) override;
    HRESULT Reset() override;
    HRESULT Clone(IEnumExplorerCommand** ppenum) override;
#pragma endregion
};
