#pragma once
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <optional>
#include <variant>

class WindowEffectHelper
{
	winrt::WinUI3Package::WindowEx m_window{ nullptr };
	winrt::Microsoft::UI::Xaml::Controls::Panel m_backgroundElement{ nullptr };
	int cachedEffect{};
	winrt::Microsoft::UI::Xaml::FrameworkElement::ActualThemeChanged_revoker m_themeChangedRevoker;
	void init();
	void UpdateBackgroundForActualTheme();
public:
	void SetTarget(
		winrt::WinUI3Package::WindowEx window,
		winrt::Microsoft::UI::Xaml::Controls::Panel backgroundElement
	);

	void SetEffect(int value);
	void SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

	void SetListenThemeChange();
};
