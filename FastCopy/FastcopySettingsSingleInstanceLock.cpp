#include "pch.h"
#include "FastcopySettingsSingleInstanceLock.h"
#include "WindowHelper.h"

FastcopySettingsSingleInstanceLock::FastcopySettingsSingleInstanceLock(bool owning)
{
	try_create(L"FastCopySettingsLock", owning ? CREATE_MUTEX_INITIAL_OWNER : 0);
}

void FastcopySettingsSingleInstanceLock::SetWindow(winrt::WinUI3Package::WindowEx const& window)
{
	auto const hwnd = reinterpret_cast<HWND>(window.Hwnd());
	SetProp(hwnd, WindowPropName, hwnd);
	window.Closed([hwnd](auto&&, auto&&) { RemoveProp(hwnd, WindowPropName); });
}

void FastcopySettingsSingleInstanceLock::ActivateWindow()
{
	EnumWindows(
		[](HWND hwnd, LPARAM) -> BOOL
		{
			if (!GetProp(hwnd, WindowPropName))
				return TRUE;

			if (IsIconic(hwnd))
				ShowWindow(hwnd, SW_RESTORE);
			BringToForeground(hwnd);
			return FALSE;
		},
		0
	);
}
