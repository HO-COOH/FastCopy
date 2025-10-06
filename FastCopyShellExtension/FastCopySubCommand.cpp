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
#include <algorithm>
#include "Registry.h"

void FastCopySubCommand::recordFilesImpl(IShellItemArray* selection)
{
    Recorder recorder{ m_op };
    for (auto item : ShellItemArray{ selection })
    {
        if (item.GetPtr())
            recorder << item;
    }
}

void FastCopySubCommand::callMainProgramImpl(std::wstring_view arg)
{
    std::wstring argTransform{ arg };
    std::transform(argTransform.begin(), argTransform.end(), argTransform.begin(), [](wchar_t c) { return c == L'\\' ? L'/' : c; });
    auto cmd = std::format(LR"(fastcopy://"{}"|{})", argTransform, Registry::Record());
#if (defined _DEBUG) || (defined DEBUG)
    OutputDebugString(cmd.data());
#endif
    ShellExecute(
        NULL,
        L"open",
        cmd.data(),
        nullptr,
        nullptr,
        SW_SHOW
    );
}

FastCopySubCommand::FastCopySubCommand(CopyOperation op) : m_op{ op }
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
        case CopyOperation::Paste:
            *cmdState = Recorder::HasRecord() &&
                (!selection || ShellItemArray{ selection }.size() == 0) ? ECS_ENABLED : ECS_HIDDEN;
            break;
        case CopyOperation::Delete:
            *cmdState = ECS_DISABLED;
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
            if (auto currentForegroundExplorer = ShellWindows::GetForegroundExplorer())
            {
                constexpr static std::wstring_view protocolPrefix{ L"file:///" };
                auto path = currentForegroundExplorer->LocationURL();
                std::wstring_view pathView{ path.get() };
                assert(pathView.starts_with(protocolPrefix));

                callMainProgramImpl(pathView.substr(protocolPrefix.size()));
                return S_OK;
            }

            ShellItem psi{ selection ? ShellItemArray{ selection }[0] : m_parent };
            callMainProgramImpl(psi.GetDisplayName());
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
