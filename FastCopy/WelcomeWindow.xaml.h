#pragma once

#include "WelcomeWindow.g.h"
#include <include/PropertyChangeHelper.hpp>

namespace winrt::FastCopy::implementation
{
    struct WelcomeWindow : WelcomeWindowT<WelcomeWindow>, MvvmHelper::PropertyChangeHelper<WelcomeWindow>
    {
        WelcomeWindow();
        void Button_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void KillExplorerButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::FastCopy::RobocopyCheckResult CheckResult();
    private:
        winrt::FastCopy::RobocopyCheckResult m_checkResult = winrt::FastCopy::RobocopyCheckResult::Checking;
        void checkResult(winrt::FastCopy::RobocopyCheckResult value);
        void checkRobocopy();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct WelcomeWindow : WelcomeWindowT<WelcomeWindow, implementation::WelcomeWindow>
    {
    };
}
