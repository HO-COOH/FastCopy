#pragma once

#include "WelcomeWindow.g.h"

class WindowEffectHelper;

namespace winrt::FastCopy::implementation
{
    struct WelcomeWindow : WelcomeWindowT<WelcomeWindow>
    {
		WelcomeWindow(WindowEffectHelper& windowEffectHelper);
        void InitializeComponent();

        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void KillExplorerButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::FastCopy::RobocopyCheckResult CheckResult();
    private:
        winrt::FastCopy::RobocopyCheckResult m_checkResult = winrt::FastCopy::RobocopyCheckResult::Checking;
		WindowEffectHelper& m_windowEffectHelper;

        void revealSubmitIssues();
        void checkResult(winrt::FastCopy::RobocopyCheckResult value);
        void checkRobocopy();
    };
}
