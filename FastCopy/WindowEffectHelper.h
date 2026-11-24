#pragma once
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <optional>
#include <variant>

namespace winrt::FastCopy::implementation
{
	struct SettingsViewModel;
}

class WindowEffectHelper
{
	winrt::WinUI3Package::WindowEx m_window{ nullptr };
	winrt::Microsoft::UI::Xaml::Controls::Panel m_backgroundElement{ nullptr };
	winrt::FastCopy::implementation::SettingsViewModel& m_settingsViewModel;
	int cachedEffect{};
	winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker m_themeChangedRevoker;
	void init();
	void UpdateBackgroundForActualTheme();
public:
	explicit WindowEffectHelper(winrt::FastCopy::implementation::SettingsViewModel& settingsViewModel);

	void SetTarget(
		winrt::WinUI3Package::WindowEx window,
		winrt::Microsoft::UI::Xaml::Controls::Panel backgroundElement
	);

	void SetEffect(int value);
	void SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

	void SetListenThemeChange();
};
