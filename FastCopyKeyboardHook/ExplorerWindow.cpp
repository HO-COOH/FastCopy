#include "ExplorerWindow.h"
#include "ExplorerFolder.h"
#include "ShellWindows.h"
#include <objbase.h>
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <ShlObj_core.h>
#include <algorithm>
#include <array>
#include <cwchar>
#include <cwctype>
#include <string>

static std::array<wchar_t, 256> getWindowClass(HWND window)
{
    std::array<wchar_t, 256> buffer{};
    GetClassNameW(window, buffer.data(), static_cast<int>(buffer.size()));
    return buffer;
}

static bool hasWindowClass(HWND window, auto&& classNames)
{
    return std::ranges::any_of(classNames, [className = getWindowClass(window)](auto const* candidate)
    {
        return std::wcscmp(className.data(), candidate) == 0;
    });
}

// Desktop is a valid paste target even though it is not an Explorer frame, and it has no shell view either,
static bool isDesktopHwnd(HWND window)
{
    constexpr std::array classNames{ L"Progman", L"WorkerW" };
    return hasWindowClass(window, classNames);
}

static bool isTextEntryWindow(HWND window)
{
    auto className = getWindowClass(window);
    std::ranges::transform(className, className.begin(), [](wchar_t value) { return std::towlower(value); });
    auto const* text = className.data();
    return std::wcsstr(text, L"edit") != nullptr ||
        std::wcsstr(text, L"richedit") != nullptr ||
        std::wcscmp(text, L"combobox") == 0;
}

// Which folder the Explorer frame is showing. Enumeration failures throw
// (wil::ResultException) and are caught at the outer boundary; entries that are not
// this frame are skipped, and a window whose active view does not support the older
// IFolderView interfaces falls back to the browser location URL.
static std::optional<std::filesystem::path> getActiveTabFolder(HWND frameWindow, GUITHREADINFO const& threadInfo)
{
    ShellWindows shellWindows;
    auto const count = shellWindows.Count();

    wil::com_ptr<IWebBrowser2> locationFallback;
    for (long index = 0; index < count; ++index)
    {
        auto browser = shellWindows.Item(index);
        if (browser.HWND() != frameWindow)
        {
            continue;
        }

        auto const shellView = ExplorerFolder::ActiveView(browser.Get());
        HWND viewWindow{};

        // Tabs of one window share the frame, so the focused view is what picks out the
        // tab the user is actually looking at.
        if (shellView && SUCCEEDED(shellView->GetWindow(&viewWindow)) &&
            (threadInfo.hwndFocus == viewWindow || IsChild(viewWindow, threadInfo.hwndFocus)))
        {
            if (auto folder = ExplorerFolder::FromShellView(shellView.get()))
            {
                return folder;
            }

            // Only one view holds the focus, so no later entry can be this tab, and its
            // own location outranks any other entry's.
            locationFallback = browser.Get();
            break;
        }

        if (!locationFallback)
        {
            locationFallback = browser.Get();
        }
    }

    return locationFallback ? ExplorerFolder::FromWebBrowser(locationFallback.get()) : std::nullopt;
}

bool IsExplorerWindow(HWND window)
{
    if (!window)
    {
        return false;
    }

    constexpr std::array explorerClassNames{ L"CabinetWClass", L"ExploreWClass" };
    return isDesktopHwnd(window) || hasWindowClass(window, explorerClassNames);
}

std::optional<std::filesystem::path> GetExplorerFolder(HWND expectedForegroundWindow)
{
    if (GetForegroundWindow() != expectedForegroundWindow || !IsExplorerWindow(expectedForegroundWindow))
    {
        return std::nullopt;
    }

    GUITHREADINFO threadInfo{ sizeof(threadInfo) };
    auto const foregroundThread = GetWindowThreadProcessId(expectedForegroundWindow, nullptr);
    if (!GetGUIThreadInfo(foregroundThread, &threadInfo) || isTextEntryWindow(threadInfo.hwndFocus))
    {
        return std::nullopt;
    }

    // The desktop has no shell view to resolve; its folder is the user's desktop.
    if (isDesktopHwnd(expectedForegroundWindow))
    {
        wil::unique_cotaskmem_string desktopPath;
        THROW_IF_FAILED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, wil::out_param(desktopPath)));
        return std::filesystem::path{ desktopPath.get() };
    }

    return getActiveTabFolder(expectedForegroundWindow, threadInfo);
}
