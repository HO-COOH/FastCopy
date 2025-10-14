#pragma once
#include <winrt/Windows.Graphics.h>
#include "AnimatedValue.h"
#include "StoryboardWrapper.h"

class AnimatedWindowSize
{

public:
    AnimatedWindowSize(HWND hwnd);

    void Resize(winrt::Windows::Graphics::SizeInt32 size);

    winrt::Windows::Graphics::SizeInt32 ActualSize() { return m_currentSize; }

    constexpr static winrt::Windows::Graphics::SizeInt32 Sizes[]
    {
        { 450, 120 },   //No duplicate size
        { 540, 400 },   //1 duplicate
        { 540, 495 },   //2 duplicates  
        { 540, 595 }    //3+ duplicates
    };

protected:
    //Cache it to improve efficiency so that we dont' resize window every time vector is changed
    winrt::Windows::Graphics::SizeInt32 m_currentSize = Sizes[0];

    /**
     * @brief Play Window animation, values from m_currentSize -> targetSize
     */
    void playWindowAnimation(winrt::Windows::Graphics::SizeInt32 targetSize);
private:
    AnimatedValue m_heightValue;
    AnimatedValue m_widthValue;

    constexpr static std::chrono::milliseconds WindowAnimationDuration{ 500 };

    StoryboardWrapper m_storyboardWrapper;

    HWND m_hwnd;
    UINT m_dpi = 96;
};
