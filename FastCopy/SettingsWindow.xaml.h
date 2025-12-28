#pragma once
#include "SettingsWindow.g.h"
#include <include/PropertyChangeHelper.hpp>

namespace winrt::FastCopy::implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow>, MvvmHelper::PropertyChangeHelper<SettingsWindow>
    {
        SettingsWindow();

        winrt::FastCopy::SettingsViewModel ViewModel();
        void ThemeSelectionComboBox_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void WindowEx_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void RenameSuffixTextBox_TextChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const& e);

    private:
        void isRenameSuffixInvalid(bool value);
        static void playTextRevealAnimation(winrt::Microsoft::UI::Xaml::Controls::TextBlock const& textBlock);
    public:
        void TextBlock_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        
        static winrt::Microsoft::UI::Xaml::Visibility NotToVisibility(bool value);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow, implementation::SettingsWindow>
    {
    };
}
