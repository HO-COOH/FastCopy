#pragma once

#include "CopyDialog.g.h"
#include "RobocopyViewModel.g.h"

namespace winrt::FastCopy::implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog>
    {
        CopyDialog();

        winrt::FastCopy::RobocopyViewModel ViewModel();
        void HyperlinkButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        static constexpr double PercentConvert(double value)
        {
            return value * 100.0;
        }

        static winrt::hstring GetTextFromKeyboardAccelerators(winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator> const& keys);
        constexpr static winrt::Microsoft::UI::Xaml::Visibility StringToVisibility(winrt::hstring const& value)
        {
            return value.empty() ? winrt::Microsoft::UI::Xaml::Visibility::Collapsed : winrt::Microsoft::UI::Xaml::Visibility::Visible;
        }

        static winrt::Microsoft::UI::Xaml::Style MenuFlyoutItemNoSubItemStyle;
        static winrt::Microsoft::UI::Xaml::Style MenuFlyoutItemHasSubItemStyle;
        static winrt::Microsoft::UI::Xaml::Style MenuFlyoutSubItemStyle;
    private:
        winrt::WinUI3Package::DependentValue m_height;
        winrt::Windows::Graphics::SizeInt32 m_originalWindowSize;
        bool m_showSpeedGraph = false;
		bool m_speedGraphLoaded = false;
        winrt::event_token m_speedUpdateRevoker;

        void applyTheme(int themeIndex);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog, implementation::CopyDialog>
    {
    };
}

