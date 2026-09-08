#pragma once
#include <wil/com.h>
#include <ExDisp.h>
#include <ShObjIdl_core.h>
#include <filesystem>
#include <optional>

// The ways a shell window tells you which folder it is showing. The keyboard hook asks
// about the foreground window, the shell extension about the browser hosting its menu,
// and both arrive at the same two interfaces. Every failure is std::nullopt: each caller
// has a fallback, and none of them can act on an HRESULT.
namespace ExplorerFolder
{
    // The view a browser or a command site is currently showing. A null argument is a
    // miss rather than a precondition, so callers need not check their site first.
    [[nodiscard]] wil::com_ptr<IShellView> ActiveView(IUnknown* browserOrSite);

    // IFolderView -> IPersistFolder2 -> pidl -> path. Misses on views that are not backed
    // by the filesystem, and on the newer views that no longer offer IFolderView.
    [[nodiscard]] std::optional<std::filesystem::path> FromShellView(IShellView* view);

    // The browser's location URL. Works where FromShellView does not, but it reports the
    // frame's folder, which on a tabbed window is not necessarily the active tab.
    [[nodiscard]] std::optional<std::filesystem::path> FromWebBrowser(IWebBrowser2* browser);

    // ActiveView followed by FromShellView.
    [[nodiscard]] std::optional<std::filesystem::path> FromSite(IUnknown* browserOrSite);
}
