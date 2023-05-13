// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "CopyDialogWindow.g.h"
#include "AnimatedValue.g.h"

namespace winrt::FastCopy::implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow>
    {
        CopyDialogWindow();

        void CopyDialog_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        constexpr static winrt::Windows::Graphics::SizeInt32 Sizes[] = { { 450,120 }, { 540,400 }, {540, 495}, {540, 595} };

        //Cache it to improve efficiency so that we dont' resize window every time vector is changed
        winrt::Windows::Graphics::SizeInt32 m_currentSize = Sizes[0];
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow, implementation::CopyDialogWindow>
    {
    };
}
