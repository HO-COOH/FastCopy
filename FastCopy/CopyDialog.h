#pragma once

#include "CopyDialog.g.h"
#include "RobocopyViewModel.g.h"

namespace winrt::FastCopy::implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog>
    {
        CopyDialog() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
            InitializeComponent();
        }

        winrt::FastCopy::RobocopyViewModel ViewModel();
        void ProgressBar_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void HyperlinkButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void PauseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog, implementation::CopyDialog>
    {
    };
}

