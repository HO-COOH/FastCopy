#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("SettingsWindow.g.cpp")
#include "SettingsWindow.g.cpp"
#endif

#include "ViewModelLocator.h"

namespace winrt::FastCopy::implementation
{
    winrt::FastCopy::SettingsViewModel SettingsWindow::ViewModel()
    {
        return ViewModelLocator::GetInstance().SettingsViewModel();
    }

    void SettingsWindow::ThemeSelectionComboBox_SelectionChanged(
        winrt::Windows::Foundation::IInspectable const& sender, 
        winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&)
    {
        switch (ThemeSelectionComboBox().SelectedIndex())
        {
            case 0: return RootPanel().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Default);
            case 1: return RootPanel().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Light);
            case 2: return RootPanel().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Dark);
        }
    }

}
