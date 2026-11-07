#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("SettingsWindow.g.cpp")
#include "SettingsWindow.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <PackageConfig.h>
#include "RenameUtils.h"
#include "TextBlockClipAnimation.h"
#include "FastcopySettingsSingleInstanceLock.h"
#include <winrt/Windows.ApplicationModel.h>
#include "WindowEffectHelper.h"
#include "SettingsViewModel.h"

namespace winrt::FastCopy::implementation
{
	SettingsWindow::SettingsWindow(WindowEffectHelper& windowEffectHelper) : m_windowEffectHelper{ windowEffectHelper }
    {
        Height(PackageConfig::GetDefaultLanguage() == L"zh-cn" ? 550 : 580);

        InitializeComponent();
        m_windowEffectHelper.SetTarget(*this, RootPanel());

        TextBlockClipAnimation{}
            .SetImplicitAnimations(ConfirmDeleteWarningText())
            .SetImplicitAnimations(InvalidSuffixTip());

        FastcopySettingsSingleInstanceLock::SetWindow(*this);
		winrt::get_self<SettingsViewModel>(ViewModel())->WindowEffectHelperPtr = &m_windowEffectHelper;
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
        if (value == m_isRenameSuffixInvalid)
            return;

		m_isRenameSuffixInvalid = value;

        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(RenameSuffixTextBox(), value ? L"Invalid" : L"Valid", false);
        InvalidSuffixTip().Visibility(value ? winrt::Microsoft::UI::Xaml::Visibility::Visible : winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
    }


    void SettingsWindow::RenameSuffixTextBox_TextChanged(
        winrt::Windows::Foundation::IInspectable const&, 
        winrt::Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&)
    {
        isRenameSuffixInvalid(!Utils::IsRenameSuffixValid(RenameSuffixTextBox().Text()));
    }

    winrt::hstring SettingsWindow::VersionString()
    {
        auto version = winrt::Windows::ApplicationModel::Package::Current().Id().Version();
        return winrt::hstring{ std::format(L"{}.{}.{}", version.Major, version.Minor, version.Build) };
    }
}

