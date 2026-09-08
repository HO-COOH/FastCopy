#include "FastCopySubCommand.h"
#include "CopyOperationNames.h"
#include <Shlwapi.h>
#include "DllIconFormatter.h"
#include <format>
#include "ShellItemArray.h"
#include "Recorder.h"
#include <cassert>
#include "ShellWindows.h"
#include "ShellItem.h"
#include "ExplorerFolder.h"
#include "FastCopyLauncher.h"
#include <algorithm>
#include "Registry.h"
#include <wil/resource.h>
#include <wil/com.h>

void FastCopySubCommand::recordFilesImpl(IShellItemArray* selection)
{
    Recorder recorder{ m_op };
    for (auto item : ShellItemArray{ selection })
    {
        if (item.Get())
            recorder << item;
    }
}

void FastCopySubCommand::callMainProgramImpl(std::wstring_view destination)
{
    // The launch result is dropped: the record is already written, and Invoke has no
    // channel to tell the user that a protocol activation failed.
    static_cast<void>(LaunchFastCopy(destination, Registry::Record()));
}

FastCopySubCommand::FastCopySubCommand(CopyOperation op, IUnknown* site) : m_op{ op }, m_site{ site }
{
}

HRESULT FastCopySubCommand::GetTitle(IShellItemArray*, PWSTR* name)
{
    auto const& nameConstants = CopyOperationNames::GetInstance();
    switch (m_op)
    {
        case CopyOperation::Copy:   return SHStrDup(nameConstants.Copy.data(), name);
        case CopyOperation::Move:   return SHStrDup(nameConstants.Move.data(), name);
        case CopyOperation::Delete: return SHStrDup(nameConstants.Delete.data(), name);
        case CopyOperation::Paste:  return SHStrDup(nameConstants.Paste.data(), name);
    }
}

HRESULT FastCopySubCommand::GetIcon(IShellItemArray*, PWSTR* icon)
{
    return SHStrDup(DllIconFormatter::GetForSubCommand(m_op).data(), icon);
}

HRESULT FastCopySubCommand::GetToolTip(IShellItemArray*, PWSTR* infoTip)
{
    *infoTip = nullptr;
    return E_NOTIMPL;
}

HRESULT FastCopySubCommand::GetCanonicalName(GUID* guidCommandName)
{
    *guidCommandName = GUID_NULL;  
    return E_NOTIMPL;
}

HRESULT FastCopySubCommand::GetState(IShellItemArray* selection, BOOL, EXPCMDSTATE* cmdState)
{
    switch (m_op)
    {
        case CopyOperation::Copy: [[fallthrough]];
        case CopyOperation::Move:
            *cmdState = ShellItemArray{ selection }.size() == 0 ? ECS_DISABLED : ECS_ENABLED;
            break;
        case CopyOperation::Delete:
            *cmdState = ShellItemArray{ selection }.size() == 0 ? ECS_HIDDEN : ECS_ENABLED;
            break;
        case CopyOperation::Paste:
            *cmdState = (Recorder::HasRecord() && (!selection || ShellItemArray{ selection }.size() == 0)) ? 
                ECS_ENABLED : 
                ECS_HIDDEN;
            break;
    }
    return S_OK;
}

HRESULT FastCopySubCommand::Invoke(IShellItemArray* selection, IBindCtx* ctx)
{
    /*
           if no files are selected, selection contains 1 element to the current invoked folder (Windows 11 only)
           On Windows 10, `selection` is `nullptr`
    */


    switch (m_op)
    {
        case CopyOperation::Copy: [[fallthrough]];
        case CopyOperation::Move: 
            recordFilesImpl(selection);
            break;
        case CopyOperation::Paste:
        {
            //On Windows 11, use selection directly
            if (ShellItemArray shellItemArray{ selection }; shellItemArray.size() != 0)
            {
                ShellItem psi{ shellItemArray[0] };
                callMainProgramImpl(psi.GetDisplayName());
                return S_OK;
            }

            //On Windows 10, selection will be nullptr, so we ask the browser hosting the menu
            if (auto const folder = ExplorerFolder::FromSite(m_site.Get()))
            {
                callMainProgramImpl(folder->native());
                return S_OK;
            }

            //Windows 10 fallback
            if (auto foregroundExplorer = ShellWindows::GetForegroundExplorer())
            {
                if (auto const folder = ExplorerFolder::FromWebBrowser(foregroundExplorer->Get()))
                {
                    callMainProgramImpl(folder->native());
                    return S_OK;
                }
            }
            break;
        }
        case CopyOperation::Delete:
            recordFilesImpl(selection);
            callMainProgramImpl(L"");
            break;
    }
    return S_OK;
}

HRESULT FastCopySubCommand::GetFlags(EXPCMDFLAGS* flags)
{
    *flags = ECF_DEFAULT; 
    return S_OK;
}

HRESULT FastCopySubCommand::EnumSubCommands(IEnumExplorerCommand** enumCommands)
{
    *enumCommands = nullptr; 
    return E_NOTIMPL;
}
