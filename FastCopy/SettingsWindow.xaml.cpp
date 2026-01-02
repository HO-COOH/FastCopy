#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("SettingsWindow.g.cpp")
#include "SettingsWindow.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <PackageConfig.h>
#include "RenameUtils.h"


namespace winrt::FastCopy::implementation
{
    SettingsWindow::SettingsWindow()
    {
        ExtendsContentIntoTitleBar(true);
        Height(PackageConfig::GetDefaultLanguage() == L"zh-cn" ? 550 : 580);
    }

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
            case 0: return RootPage().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Default);
            case 1: return RootPage().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Light);
            case 2: return RootPage().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Dark);
        }
    }

    void SettingsWindow::WindowEx_Activated(
        winrt::Windows::Foundation::IInspectable const&, 
        winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args)
    {
        TitleBarTextBlock().Opacity(args.WindowActivationState() == winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated ? 0.5f : 1.0f);
    }


    void SettingsWindow::isRenameSuffixInvalid(bool value)
    {
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(RenameSuffixTextBox(), value ? L"Invalid" : L"Valid", false);
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(RootPage(), value ? L"InvalidRenameSuffixState" : L"Normal", false);
    }


    void SettingsWindow::RenameSuffixTextBox_TextChanged(
        winrt::Windows::Foundation::IInspectable const&, 
        winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&)
    {
        isRenameSuffixInvalid(!Utils::IsRenameSuffixValid(RenameSuffixTextBox().Text()));
    }


    void SettingsWindow::ConfirmDeleteToggle_Toggled(
        winrt::Windows::Foundation::IInspectable const& sender, 
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        if (!ConfirmDeleteWarningText().IsLoaded())
            return;

        auto confirmDeleteToggle = sender.as<winrt::Microsoft::UI::Xaml::Controls::ToggleSwitch>();
        auto const isOn = confirmDeleteToggle.IsOn();
        ConfirmDeleteWarningText().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
        isOn ? playTextRevealAnimation<false>(ConfirmDeleteWarningText()) : playTextRevealAnimation<true>(ConfirmDeleteWarningText());
    }

}

