// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "CopyDialogWindow.g.h"

class WindowEffectHelper;

namespace winrt::FastCopy::implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow>
    {
        CopyDialogWindow(WindowEffectHelper& windowEffectHelper);

        void InitializeComponent();

        //The WindowSizeStates in CopyDialogWindow.xaml animate to these, keep them in sync
        constexpr static winrt::Windows::Graphics::SizeInt32 NormalSize{ 450, 120 };
        constexpr static winrt::Windows::Graphics::SizeInt32 Sizes[]
        {
            { 540, 400 },   //1 duplicate
            { 540, 495 },   //2 duplicates  
            { 540, 595 }    //3+ duplicates
        };

        static constexpr double PercentConvert(double value)
        {
            return value * 100.0;
        }

        void HyperlinkButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::FastCopy::RobocopyViewModel ViewModel();
        void WindowEx_Closed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args);


		static inline winrt::Microsoft::UI::Xaml::Style MenuFlyoutItemNoSubItemStyle{ nullptr };
        static inline winrt::Microsoft::UI::Xaml::Style MenuFlyoutItemHasSubItemStyle{ nullptr };
        static inline winrt::Microsoft::UI::Xaml::Style MenuFlyoutSubItemStyle{ nullptr };
        static winrt::hstring GetTextFromKeyboardAccelerators(winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator> const& keys);


    private:
        bool m_showSpeedGraph = false;
        bool m_failed = false;
		WindowEffectHelper& m_windowEffectHelper;

        //Picks the states of SpeedGraphStates, DuplicateFilesStates and WindowSizeStates together, as they depend on each other
        void updateVisualStates();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow, implementation::CopyDialogWindow>
    {
    };
}
