#include "pch.h"
#include "WindowHelper.h"
#include <microsoft.ui.xaml.window.h>
#include <winrt/Microsoft.UI.Interop.h>
#include "Global.h"

LONG ScaleForDpi(UINT value, UINT dpi)
{
    return static_cast<LONG>(value * (dpi / 96.0));
}

HWND GetHwnd(winrt::Microsoft::UI::Xaml::Window window)
{
    HWND hwnd{};
    window.as<IWindowNative>()->get_WindowHandle(&hwnd);
    Global::MainHwnd = hwnd;
    return hwnd;
}

winrt::Microsoft::UI::Windowing::AppWindow GetAppWindow(winrt::Microsoft::UI::Xaml::Window window)
{
    return window.AppWindow();
}

void MoveAndResizeWindow(winrt::Microsoft::UI::Xaml::Window window, winrt::Windows::Graphics::RectInt32 windowRect)
{
    auto const dpi = GetDpiForWindow(GetHwnd(window));
    windowRect.Width = ScaleForDpi(windowRect.Width, dpi);
    windowRect.Height = ScaleForDpi(windowRect.Height, dpi);
    GetAppWindow(window).MoveAndResize(windowRect);
}

void CenterWindow(winrt::Microsoft::UI::Xaml::Window window, winrt::Windows::Graphics::SizeInt32 size)
{
    auto const monitorInfo = MonitorInfo::GetPrimary();
    auto const dpi = GetDpiForWindow(GetHwnd(window));
    MoveAndResizeWindow(window, 
    {
        .X = (monitorInfo.rcWork.right - monitorInfo.rcWork.left - ScaleForDpi(size.Width, dpi)) / 2,
        .Y = (monitorInfo.rcWork.bottom - monitorInfo.rcWork.top - ScaleForDpi(size.Height, dpi)) / 2,
        .Width = size.Width,
        .Height = size.Height
    });
}

void CenterWindow(winrt::Microsoft::UI::Xaml::Window window)
{
    auto const hwnd = GetHwnd(window);
    auto const monitorInfo = MonitorInfo::GetFromWindow(hwnd);
    RECT rect;
    GetWindowRect(hwnd, &rect);
    auto const windowWidth = rect.right - rect.left;
    auto const windowHeight = rect.bottom - rect.top;
    SetWindowPos(
        hwnd,
        nullptr,
        (monitorInfo.rcWork.right - monitorInfo.rcWork.left - windowWidth) / 2,
        (monitorInfo.rcWork.bottom - monitorInfo.rcWork.top - windowHeight) / 2,
        0, 0,
        SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER
    );
}

void ResizeWindowForDpi(HWND hwnd, winrt::Windows::Graphics::SizeInt32 size, UINT dpi)
{
    size.Height = ScaleForDpi(size.Height, dpi);
    size.Width = ScaleForDpi(size.Width, dpi);
    SetWindowPos(
        hwnd,
        nullptr,
        0,
        0,
        size.Width,
        size.Height,
        SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER
    );
}

MONITORINFO MonitorInfo::GetFromPoint(POINT p)
{
    MONITORINFO info
    {
        .cbSize = sizeof(info)
    };
    GetMonitorInfo(MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST), &info);
    return info;
}

MONITORINFO MonitorInfo::GetPrimary()
{
    MONITORINFO info
    {
        .cbSize = sizeof(info)
    };
    GetMonitorInfo(MonitorFromPoint(POINT{}, MONITOR_DEFAULTTOPRIMARY), &info);
    return info;
}

MONITORINFO MonitorInfo::GetFromWindow(HWND hwnd)
{
    MONITORINFO info
    {
        .cbSize = sizeof(info)
    };
    GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &info);
    return info;
}
