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

void FastCopySubCommand::callMainProgramImpl(std::wstring_view arg)
{
    std::wstring argTransform{ arg };
    std::transform(argTransform.begin(), argTransform.end(), argTransform.begin(), [](wchar_t c) { return c == L'\\' ? L'/' : c; });
    auto cmd = std::format(LR"(fastcopy://"{}"|"{}")", argTransform, Registry::Record());
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
        case CopyOperation::Delete: [[fallthrough]];
        case CopyOperation::Move:
            *cmdState = ShellItemArray{ selection }.size() == 0 ? ECS_DISABLED : ECS_ENABLED;
            break;
        case CopyOperation::Paste:
            *cmdState = (Recorder::HasRecord() && (!selection || ShellItemArray{ selection }.size() == 0)) ? 
                ECS_ENABLED : 
                ECS_HIDDEN;
            break;
        //case CopyOperation::Delete:
        //{
        //    //if no selection, disable
        //    ShellItemArray items{ selection };
        //    if (items.size() == 0)
        //    {
        //        *cmdState = ECS_DISABLED;
        //        break;
        //    }

        //    //only enable when all items are folders
        //    bool allFolders = std::ranges::all_of(items, [](ShellItem const& item) {
        //        return std::filesystem::is_directory(item.GetDisplayName());
        //    });
        //    *cmdState = allFolders ? ECS_ENABLED : ECS_DISABLED;
        //}
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

            //On Windows 10, selection will be nullptr, so we find the active shell window
            if (m_site)
            {
                wil::com_ptr<IShellBrowser> spSB;
                if (SUCCEEDED(IUnknown_QueryService(m_site.Get(), SID_STopLevelBrowser, IID_PPV_ARGS(&spSB))))
                {
                     wil::com_ptr<IShellView> spSV;
                     if (SUCCEEDED(spSB->QueryActiveShellView(&spSV)))
                     {
                         if (auto spFV = spSV.try_query<IFolderView>())
                         {
                             wil::com_ptr<IPersistFolder2> spPF2;
                             if (SUCCEEDED(spFV->GetFolder(IID_PPV_ARGS(&spPF2))))
                             {
                                 wil::unique_cotaskmem_ptr<ITEMIDLIST> pidl;
                                 if (SUCCEEDED(spPF2->GetCurFolder(wil::out_param(pidl))))
                                 {
                                     wchar_t path[MAX_PATH];
                                     if (SHGetPathFromIDListW(pidl.get(), path))
                                     {
                                         callMainProgramImpl(path);
                                         return S_OK;
                                     }
                                 }
                             }
                         }
                     }
                }
            }

            //Windows 10 fallback
            if (auto currentForegroundExplorer = ShellWindows::GetForegroundExplorer())
            {
                constexpr static std::wstring_view protocolPrefix{ L"file:///" };
                auto path = currentForegroundExplorer->LocationURL();

                std::wstring pathUnescaped(wcslen(path.get()), L'\0');
                DWORD bufferSize = pathUnescaped.size();
                if (UrlUnescape(
                    path.get(),
                    pathUnescaped.data(),
                    &bufferSize,
                    URL_DONT_UNESCAPE_EXTRA_INFO
                ) == E_POINTER)
                {
                    pathUnescaped.resize(bufferSize + 1);
                    UrlUnescape(
                        path.get(),
                        pathUnescaped.data(),
                        &bufferSize,
                        URL_DONT_UNESCAPE_EXTRA_INFO
                    );
                }

                assert(pathUnescaped.starts_with(protocolPrefix));
                callMainProgramImpl(std::wstring{ pathUnescaped.substr(protocolPrefix.size()).data()});
                return S_OK;
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
