#pragma once
#include "SettingsWindow.g.h"
#include <include/PropertyChangeHelper.hpp>
#include "TextBlockClipAnimation.h"

namespace winrt::FastCopy::implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow>, MvvmHelper::PropertyChangeHelper<SettingsWindow>
    {
        SettingsWindow();

        winrt::FastCopy::SettingsViewModel ViewModel();
        void ThemeSelectionComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void WindowEx_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void RenameSuffixTextBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);
        void ConfirmDeleteToggle_Toggled(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        constexpr static winrt::Microsoft::UI::Xaml::Visibility NotToVisibility(bool value)
        {
            return value ? winrt::Microsoft::UI::Xaml::Visibility::Collapsed
                : winrt::Microsoft::UI::Xaml::Visibility::Visible;
        }

    private:
        void isRenameSuffixInvalid(bool value);
		bool m_isRenameSuffixInvalid{ false };
        TextBlockClipAnimation m_confirmDeleteTextAnimation;
		TextBlockClipAnimation m_invalidSuffixTipTextAnimation;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow, implementation::SettingsWindow>
    {
    };
}
