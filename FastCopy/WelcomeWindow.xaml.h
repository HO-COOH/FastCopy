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

        constexpr winrt::FastCopy::RobocopyCheckResult CheckResult() const
        {
            return m_checkResult;
        }
    private:
        winrt::FastCopy::RobocopyCheckResult m_checkResult = winrt::FastCopy::RobocopyCheckResult::Checking;
		WindowEffectHelper& m_windowEffectHelper;

        [[nodiscard]] static winrt::FastCopy::RobocopyCheckResult checkRobocopy();
    };
}
