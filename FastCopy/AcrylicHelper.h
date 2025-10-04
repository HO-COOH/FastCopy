#pragma once
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <optional>
#include <variant>

class WindowEffectHelper
{
	winrt::Microsoft::UI::Xaml::Window m_target;

	void init();
public:
	WindowEffectHelper(winrt::Microsoft::UI::Xaml::Window window = nullptr) : m_target{ window } { init(); }

	void SetTarget(winrt::Microsoft::UI::Xaml::Window window);
	void SetEffect(int value);
	void SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

	void SetListenThemeChange();
};
