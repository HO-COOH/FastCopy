#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("SettingsWindow.g.cpp")
#include "SettingsWindow.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <PackageConfig.h>
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
    }

    void SettingsWindow::InitializeComponent()
    {
        SettingsWindowT::InitializeComponent();
        m_windowEffectHelper.SetTarget(*this, RootPanel());

        TextBlockClipAnimation{}
            .SetImplicitAnimations(ConfirmDeleteWarningText())
            .SetImplicitAnimations(InvalidSuffixTip());

        FastcopySettingsSingleInstanceLock::SetWindow(*this);
        winrt::get_self<SettingsViewModel>(ViewModel())->WindowEffectHelperPtr = &m_windowEffectHelper;
    }

    winrt::FastCopy::SettingsViewModel SettingsWindow::ViewModel()
    {
        return *ViewModelLocator::GetInstance().SettingsViewModel();
    }

    void SettingsWindow::WindowEx_Activated(
        winrt::Windows::Foundation::IInspectable const&, 
        winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args)
    {
        TitleBarTextBlock().Opacity(args.WindowActivationState() == winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated ? 0.5f : 1.0f);
    }

    winrt::hstring SettingsWindow::VersionString()
    {
        auto const [major, minor, build, _] = winrt::Windows::ApplicationModel::Package::Current().Id().Version();
        return winrt::hstring{ std::format(L"{}.{}.{}", major, minor, build) };
    }
}

