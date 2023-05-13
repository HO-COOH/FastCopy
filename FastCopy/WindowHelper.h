#pragma once
#include <Windows.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.Xaml.h>

/**
 * Scale a value based on dpi
 * @param value To be scaled
 * @param dpi The monitor dpi
 * @return scaled value
 */
LONG ScaleForDpi(UINT value, UINT dpi);


struct MonitorInfo
{
	static MONITORINFO GetFromPoint(POINT p);
	static MONITORINFO GetPrimary();
};

/**
 * Get HWND from a xaml window
 */
HWND GetHwnd(winrt::Microsoft::UI::Xaml::Window& window);

/**
 * Get AppWindow from a xaml window
 */
winrt::Microsoft::UI::Windowing::AppWindow GetAppWindow(winrt::Microsoft::UI::Xaml::Window window);


/**
 * Move and resize window, with dpi scaled to primary monitor
 */
void MoveAndResizeWindow(winrt::Microsoft::UI::Xaml::Window window, winrt::Windows::Graphics::RectInt32 windowRect);

/**
 * Center and resize window, with dpi scaled to primary monitor
 */
void CenterWindow(winrt::Microsoft::UI::Xaml::Window window, winrt::Windows::Graphics::SizeInt32 size);


void ResizeWindow(winrt::Microsoft::UI::Xaml::Window window, winrt::Windows::Graphics::SizeInt32 size);

