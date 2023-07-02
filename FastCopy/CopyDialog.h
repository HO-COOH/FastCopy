#pragma once

#include "CopyDialog.g.h"
#include "RobocopyViewModel.g.h"
#include "DependentValue.g.h"
namespace winrt::FastCopy::implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog>
    {
        CopyDialog();

        winrt::FastCopy::RobocopyViewModel ViewModel();
        void ProgressBar_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void HyperlinkButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PauseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void CheckBox_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void MainPanel_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);

        /**
         * .
         */
        void ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void ShowMenu(winrt::hstring path, winrt::Microsoft::UI::Xaml::Controls::MenuFlyout flyout);
    private:
        winrt::FastCopy::DependentValue m_height;
        winrt::Windows::Graphics::SizeInt32 m_originalWindowSize;
        bool m_showSpeedGraph = false;
        winrt::event_token m_speedUpdateRevoker;
    public:
        void MenuFlyout_Opening(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& e);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog, implementation::CopyDialog>
    {
    };
}

