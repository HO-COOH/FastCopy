#include "ExplorerFolder.h"
#include <wil/resource.h>
#include <PathCch.h>
#include <ShlObj_core.h>
#include <Shlwapi.h>
#include <array>

wil::com_ptr<IShellView> ExplorerFolder::ActiveView(IUnknown* browserOrSite)
{
    if (!browserOrSite)
    {
        return {};
    }

    // IUnknown_QueryService asks for IServiceProvider on the way through, which is what
    // makes this work for a plain IWebBrowser2 as well as for a command site.
    wil::com_ptr<IShellBrowser> shellBrowser;
    if (FAILED(IUnknown_QueryService(browserOrSite, SID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))))
    {
        return {};
    }

    wil::com_ptr<IShellView> shellView;
    if (FAILED(shellBrowser->QueryActiveShellView(shellView.put())))
    {
        return {};
    }
    return shellView;
}

std::optional<std::filesystem::path> ExplorerFolder::FromShellView(IShellView* view)
{
    wil::com_ptr<IFolderView> folderView;
    if (!view || FAILED(view->QueryInterface(folderView.put())))
    {
        return std::nullopt;
    }

    wil::com_ptr<IPersistFolder2> persistFolder;
    if (FAILED(folderView->GetFolder(IID_PPV_ARGS(persistFolder.put()))))
    {
        return std::nullopt;
    }

    wil::unique_cotaskmem_ptr<ITEMIDLIST> pidl;
    if (FAILED(persistFolder->GetCurFolder(wil::out_param(pidl))))
    {
        return std::nullopt;
    }

    // Ex rather than SHGetPathFromIDListW: a folder can sit past MAX_PATH, and the
    // shorter form has nowhere to report that it truncated.
    std::array<wchar_t, PATHCCH_MAX_CCH> path{};
    auto const converted = SHGetPathFromIDListEx(
        pidl.get(),
        path.data(),
        static_cast<DWORD>(path.size()),
        GPFIDL_DEFAULT);
    return converted ? std::optional{ std::filesystem::path{ path.data() } } : std::nullopt;
}

std::optional<std::filesystem::path> ExplorerFolder::FromWebBrowser(IWebBrowser2* browser)
{
    wil::unique_bstr location;
    if (!browser || FAILED(browser->get_LocationURL(location.put())) || !location)
    {
        return std::nullopt;
    }

    // PathCreateFromUrlW unescapes and strips the scheme in one step, and fails outright
    // on a location that is no filesystem path at all - a search results view, say.
    std::array<wchar_t, PATHCCH_MAX_CCH> path{};
    auto length = static_cast<DWORD>(path.size());
    return SUCCEEDED(PathCreateFromUrlW(location.get(), path.data(), &length, 0))
        ? std::optional{ std::filesystem::path{ path.data() } }
        : std::nullopt;
}

std::optional<std::filesystem::path> ExplorerFolder::FromSite(IUnknown* browserOrSite)
{
    return FromShellView(ActiveView(browserOrSite).get());
}
