#pragma once
#include <wil/resource.h>
#include <winrt/WinUI3Package.h>

class FastcopySettingsSingleInstanceLock : public wil::unique_mutex_nothrow
{
	constexpr static auto WindowPropName = L"FastCopySettingsWindow";
public:
	FastcopySettingsSingleInstanceLock(bool owning = true);

	/**
	 * @brief Mark window as the window of the settings instance, so another launch can find and activate it
	 */
	static void SetWindow(winrt::WinUI3Package::WindowEx const& window);

	/**
	 * @brief Restore and bring the window of the running settings instance to front
	 */
	static void ActivateWindow();
};
