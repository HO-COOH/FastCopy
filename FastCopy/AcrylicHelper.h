#pragma once
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
class AcrylicHelper
{
	winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController m_acrylicController;
	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration m_configurationSource;

	static void EnsureWindowsSystemDispatcherQueueController();
	winrt::Microsoft::UI::Xaml::Window& m_window;
public:
	AcrylicHelper(winrt::Microsoft::UI::Xaml::Window& window);
	bool trySetAcrylicBackdrop();
	void setConfigurationSourceTheme();
};

class MicaHelper
{
	winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController m_micaController;
	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration m_configurationSource;
	winrt::Microsoft::UI::Xaml::Window& m_window;
	static void EnsureWindowsSystemDispatcherQueueController();
public:
	MicaHelper(winrt::Microsoft::UI::Xaml::Window& window);
	bool trySetMicaBackdrop();
	void setConfigurationSourceTheme();
};
