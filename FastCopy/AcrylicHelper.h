#pragma once
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <optional>
#include <variant>
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
	~AcrylicHelper();
	winrt::Microsoft::UI::Xaml::Window& GetWindow() { return m_window; }
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
	~MicaHelper();
	winrt::Microsoft::UI::Xaml::Window& GetWindow() { return m_window; }
private:
	winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController m_micaController;
	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration m_configurationSource;
	winrt::Microsoft::UI::Xaml::Window& m_window;
	static void EnsureWindowsSystemDispatcherQueueController();
	bool trySetMicaBackdrop(MicaParameter const& parameter);
	void setConfigurationSourceTheme();
};

class WindowEffectHelper
{
	winrt::Microsoft::UI::Xaml::Window m_target;
	std::variant<MicaHelper, AcrylicHelper, std::nullptr_t> m_helper = nullptr;

	template<typename EffectType, typename Parameter, typename Func = std::nullptr_t>
	void trySetEffect(Parameter&& parameter, Func&& onFail)
	{
		if (!m_target)
			return;

		m_helper.emplace<EffectType>(m_target, std::move(parameter));
	}

	void init();
public:
	WindowEffectHelper(winrt::Microsoft::UI::Xaml::Window window = nullptr) : m_target{ window } { init(); }

	void SetTarget(winrt::Microsoft::UI::Xaml::Window window)
	{
		m_target = window;
		init();
	}

	void SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme);

	void SetListenThemeChange();

	template<typename Func = std::nullptr_t>
	void TrySetAcrylic(AcrylicHelper::AcrylicParameter&& parameter = {}, Func && onFail = nullptr)
	{
		trySetEffect<AcrylicHelper>(std::move(parameter), std::move(onFail));
	}

	template<typename Func = std::nullptr_t>
	void TrySetMica(MicaHelper::MicaParameter&& parameter = {}, Func&& onFail = nullptr)
	{
		trySetEffect<MicaHelper>(std::move(parameter), std::move(onFail));
	}

	void Reset()
	{
		m_helper = nullptr;
	}
};
