#pragma once
#include "SettingsWindow.g.h"
#include <include/PropertyChangeHelper.hpp>

class WindowEffectHelper;

namespace winrt::FastCopy::implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow>, MvvmHelper::PropertyChangeHelper<SettingsWindow>
    {
        SettingsWindow(WindowEffectHelper& windowEffectHelper);
        void InitializeComponent();
        winrt::FastCopy::SettingsViewModel ViewModel();
        void WindowEx_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);

		static winrt::hstring VersionString();
    private:
		WindowEffectHelper& m_windowEffectHelper;
    };
}

namespace winrt::FastCopy::factory_implementation
{
	struct SettingsWindow : SettingsWindowT<SettingsWindow, implementation::SettingsWindow>
	{
	};
}
