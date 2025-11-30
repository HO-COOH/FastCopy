#include "FastCopyRootCommand.h"
#include <Shlwapi.h>
#include "ShellItemArray.h"
#include "CopyOperation.h"
#include <array>
#include <algorithm>
#include "DllIconFormatter.h"
#include "FastCopySubCommand.h"
#include "Recorder.h"
#include "PackageConfig.h"

bool FastCopyRootCommand::hasInvokedCopyOrCut()
{
    return Recorder::HasRecord(); 
}

void FastCopyRootCommand::init(IShellItemArray* items)
{
    if (m_hasInit)
    {
        FC_LOG_WARN(L"FastCopyRootCommand has already initialized.");
        return;
    }

    DWORD const count = ShellItemArray{ items }.size();
    bool showPasteCommand = hasInvokedCopyOrCut();

    FC_LOG_INFO(L"Site: {:p}, items count: {}, showPasteCommand: {}",
        static_cast<const void*>(m_site.Get()), count, showPasteCommand);

    m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Copy, m_site.Get()));
    m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Move, m_site.Get()));
    if (showPasteCommand)
        m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Paste, m_site.Get()));
    m_subCommands.emplace_back(Microsoft::WRL::Make<FastCopySubCommand>(CopyOperation::Delete, m_site.Get()));
    m_hasInit = true;

    FC_LOG_INFO(L"Init success.");
}

HRESULT FastCopyRootCommand::SetSite(IUnknown* pUnkSite)
{
    FC_LOG_INFO(L"FastCopyRootCommand::SetSite: pUnkSite={:p}, oldValue={:p}."
        , static_cast<const void*>(pUnkSite), static_cast<const void*>(m_site.Get()));
    m_site = pUnkSite;
    return S_OK;
}

HRESULT FastCopyRootCommand::GetSite(REFIID riid, void** ppvSite)
{
    FC_LOG_INFO(L"FastCopyRootCommand::GetSite: riid={:08X}-{:04X}, ppvSite={:p}."
        , riid.Data1, riid.Data2, static_cast<const void*>(ppvSite));
    return m_site.CopyTo(riid, ppvSite);
}

HRESULT FastCopyRootCommand::GetTitle(IShellItemArray*, PWSTR* name)
{
    auto hr = SHStrDup(PackageConfig::PackageName, name);
    FC_LOG_DEBUG(L"GetTitle: hr=0x{:08X}, title='{}'", hr, *name ? *name : L"(null)");
    return hr;
}

HRESULT FastCopyRootCommand::GetIcon(IShellItemArray*, PWSTR* icon)
{
    return SHStrDup(DllIconFormatter::GetForRootCommand().data(), icon);
}

HRESULT FastCopyRootCommand::GetToolTip(IShellItemArray*, PWSTR* infoTip)
{
    *infoTip = {};
    return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::GetCanonicalName(GUID* guidCommandName)
{
    *guidCommandName = GUID_NULL; 
    return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::GetState(IShellItemArray* selection, BOOL okToBeSlow, EXPCMDSTATE* cmdState)
{
    init(selection);

    FC_LOG_DEBUG(L"GetState: selection size: {}, has invoked copy or cut: {}, sub commands size: {}.", 
        ShellItemArray{ selection }.size(), hasInvokedCopyOrCut(), m_subCommands.size());

    if (m_hasInit && !m_subCommands.empty())
        *cmdState = ECS_ENABLED;
    else
        *cmdState = ECS_DISABLED;

    FC_LOG_DEBUG(L"GetState: cmd state:{}.", *cmdState);
    return S_OK;
}

HRESULT FastCopyRootCommand::GetFlags(EXPCMDFLAGS* flags)
{
    *flags = ECF_HASSUBCOMMANDS;
    return S_OK;
}

HRESULT FastCopyRootCommand::Invoke(IShellItemArray* selection, IBindCtx*)
{
    return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::EnumSubCommands(IEnumExplorerCommand** enumCommands)
{
    FC_LOG_DEBUG(L"EnumSubCommands: count={}", m_subCommands.size());

    m_subCommandIter = m_subCommands.begin();
    // Do we need to call AddRef() manually here?
    // DllCanUnloadNow() return false with non zero positive value of ref count.
    // See: https://learn.microsoft.com/en-us/cpp/atl/queryinterface?view=msvc-170
    // document says:
    // If the object supports that interface, QueryInterface retrieves a pointer to the interface,
    // while also calling AddRef. (Caution!!! It will call AddRef by itself.)
    // Otherwise, it returns the E_NOINTERFACE error code.
    AddRef();
    auto hr = QueryInterface(IID_PPV_ARGS(enumCommands));
    FC_LOG_DEBUG(L"EnumSubCommands QI(IEnumExplorerCommand) hr=0x{:08X}", hr);
    return hr;
}

HRESULT FastCopyRootCommand::Next(ULONG numItemsToFetch, IExplorerCommand** pUICommand, ULONG* numItemsFetched)
{
    ULONG fetched = 0;

    for (
        ULONG i = 0;
        (i < numItemsToFetch) &&
        (this->m_subCommandIter != this->m_subCommands.cend());
        ++i)
    {
        m_subCommandIter->CopyTo(&pUICommand[i]);
        ++m_subCommandIter;
        ++fetched;
    }

    if (numItemsFetched)
    {
        *numItemsFetched = fetched;
    }

    return (fetched == numItemsToFetch) ? S_OK : S_FALSE;
}

HRESULT FastCopyRootCommand::Skip(ULONG)
{
    return E_NOTIMPL;
}

HRESULT FastCopyRootCommand::Reset()
{
    m_subCommandIter = m_subCommands.begin();
    return S_OK;
}

HRESULT FastCopyRootCommand::Clone(IEnumExplorerCommand** ppenum)
{
    *ppenum = nullptr;
    return E_NOTIMPL;
}
