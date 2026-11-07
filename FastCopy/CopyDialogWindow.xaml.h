// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "CopyDialogWindow.g.h"

namespace winrt::FastCopy::implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow>
    {
        CopyDialogWindow();

        constexpr static winrt::Windows::Graphics::SizeInt32 NormalSize{ 450, 120 };
        constexpr static winrt::Windows::Graphics::SizeInt32 Sizes[]
        {
            { 540, 400 },   //1 duplicate
            { 540, 495 },   //2 duplicates  
            { 540, 595 }    //3+ duplicates
        };

        winrt::FastCopy::RobocopyViewModel ViewModel();
        void WindowEx_Closed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow, implementation::CopyDialogWindow>
    {
    };
}
