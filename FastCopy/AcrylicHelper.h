#pragma once
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <optional>
class AcrylicHelper
{
public:
	struct AcrylicParameter
	{
		std::optional<winrt::Windows::UI::Color> fallbackColor;
		std::optional<winrt::Windows::UI::Color> tintColor;
		std::optional<float> tintOpacity;
		std::optional<float> luminosityOpacity;
	};
	AcrylicHelper(winrt::Microsoft::UI::Xaml::Window& window, AcrylicParameter acrylicParameter = {});
private:
	winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController m_acrylicController;
	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration m_configurationSource;

	static void EnsureWindowsSystemDispatcherQueueController();
	winrt::Microsoft::UI::Xaml::Window& m_window;

	bool trySetAcrylicBackdrop(AcrylicParameter const& parameter);
	void setConfigurationSourceTheme();
};

class MicaHelper
{
public:
	struct MicaParameter
	{
		std::optional<winrt::Windows::UI::Color> fallbackColor;
		std::optional<winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind> kind;
		std::optional<float> luminosityOpacity;
		std::optional<winrt::Windows::UI::Color> tintColor;
		std::optional<float> tintOpacity;
	};

	MicaHelper(winrt::Microsoft::UI::Xaml::Window& window, MicaParameter micaParameter = {});
private:
	winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController m_micaController;
	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration m_configurationSource;
	winrt::Microsoft::UI::Xaml::Window& m_window;
	static void EnsureWindowsSystemDispatcherQueueController();
	bool trySetMicaBackdrop(MicaParameter const& parameter);
	void setConfigurationSourceTheme();
};
