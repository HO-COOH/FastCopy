#include "pch.h"
#include "AnimatedWindowSize.h"
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include "WindowHelper.h"

AnimatedWindowSize::AnimatedWindowSize(HWND hwnd) : m_hwnd{ hwnd }
{
}

void AnimatedWindowSize::Resize(winrt::Windows::Graphics::SizeInt32 size)
{
    playWindowAnimation(size);
    m_currentSize = size;
}
void AnimatedWindowSize::playWindowAnimation(winrt::Windows::Graphics::SizeInt32 targetSize)
{
    winrt::Microsoft::UI::Xaml::Media::Animation::CircleEase f;
    f.EasingMode(winrt::Microsoft::UI::Xaml::Media::Animation::EasingMode::EaseInOut);
    m_heightValue
        .From(m_currentSize.Height)
        .To(targetSize.Height)
        .EasingFunction(f)
        .Duration(WindowAnimationDuration);
    m_widthValue
        .From(m_currentSize.Width)
        .To(targetSize.Width)
        .EasingFunction(f)
        .Duration(WindowAnimationDuration);

    m_dpi = GetDpiForWindow(m_hwnd);

    (m_storyboardWrapper << m_heightValue << m_widthValue).Begin();

    m_heightValue.OnValueChange([this](auto, double heightValue) {
        auto const widthValue = (int)m_widthValue;
        if (widthValue != 0 && heightValue != 0)
            ResizeWindowForDpi(m_hwnd, { .Width = widthValue, .Height = static_cast<int>(heightValue) }, m_dpi);
        });
}