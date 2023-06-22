// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "CopyDialogWindow.g.h"
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include "DependentValue.g.h"
#include "AnimatedValue.h"
#include <optional>

namespace winrt::FastCopy::implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow>
    {
        CopyDialogWindow();

        void Resize(winrt::Windows::Graphics::SizeInt32 size);

        winrt::Windows::Graphics::SizeInt32 ActualSize() { return m_currentSize; }
    private:
        constexpr static winrt::Windows::Graphics::SizeInt32 Sizes[]
        { 
            { 450, 120 },  //No duplicate size
            { 540, 400 },  //1 duplicate
            { 540, 495 },   //2 duplicates  
            { 540, 595 }    //3+ duplicates
        };

        //Cache it to improve efficiency so that we dont' resize window every time vector is changed
        winrt::Windows::Graphics::SizeInt32 m_currentSize = Sizes[0];
    
        AnimatedValue m_heightValue;
        AnimatedValue m_widthValue;

        constexpr static std::chrono::milliseconds WindowAnimationDuration{ 500 };
        
        /**
         * @brief Play Window animation, values from m_currentSize -> targetSize
         */
        void playWindowAnimation(winrt::Windows::Graphics::SizeInt32 targetSize);

        StoryboardWrapper m_storyboardWrapper;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow, implementation::CopyDialogWindow>
    {
    };
}
