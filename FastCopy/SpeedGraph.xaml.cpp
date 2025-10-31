// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "SpeedGraph.xaml.h"
#if __has_include("SpeedGraph.g.cpp")
#include "SpeedGraph.g.cpp"
#endif
#include <winrt/Microsoft.UI.Xaml.Media.Animation.h>
#include <format>
#include "ReadableUnitConverter.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    winrt::Microsoft::UI::Xaml::Media::PointCollection SpeedGraph::Points()
    {
        return m_graphData.Points();
	}

    void SpeedGraph::SetSpeed(double percent, uint64_t speed)
    {
		auto const [newScaleRatio, needAnimation] = m_graphData.SetSpeed(percent, speed);
        
        if (newScaleRatio != 1.0f)
            resizeGraphPoint(newScaleRatio);
        
        if (needAnimation)
        {
            AverageSpeedText().Text(ReadableUnitConverter::Speed::ToString<wchar_t>(speed).data());
            makeAnimation();
        }

        if (!m_hasData)
        {
            GraphGrid().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
            SpeedGraphNoDataAvailableText().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
            //m_graphData.NewSize(ActualSize());
            m_hasData = true;
        }
    }
    void SpeedGraph::resizeGraphPoint(float ratio)
    {
        m_graphData.SetRatio(ratio);
        ScaleDownAnimation().To(m_graphData.GetRatio());
        ScaleDown().Begin();
    }

    void SpeedGraph::makeAnimation(float y)
    {
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard speedLineAndTextAnimation;

        winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation lineTranslate;
        winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation textTranslate;

        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(lineTranslate, LineTranslate());
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(lineTranslate, L"Y");
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(textTranslate, TextTranslate());
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(textTranslate, L"Y");

        lineTranslate.Duration(speedLineAndTextAnimationDuration);
        textTranslate.Duration(speedLineAndTextAnimationDuration);

        winrt::Microsoft::UI::Xaml::Media::Animation::ExponentialEase ease;
        ease.Exponent(7);
        ease.EasingMode(winrt::Microsoft::UI::Xaml::Media::Animation::EasingMode::EaseOut);
        lineTranslate.EasingFunction(ease);
        textTranslate.EasingFunction(ease);


        lineTranslate.To(y);
        textTranslate.To(y);
        speedLineAndTextAnimation.Children().ReplaceAll({ lineTranslate, textTranslate });
        speedLineAndTextAnimation.Begin();
    }

    void SpeedGraph::makeAnimation()
    {
        auto const p = m_graphData.GetLastPoint();
        auto const y = m_graphData.Height() - ((m_graphData.Height() - p.Y) * m_graphData.GetRatio());
        makeAnimation(y);
    }

    void SpeedGraph::CanvasControl_Draw(winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender, winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args)
    {
        auto drawingSession = args.DrawingSession();

        auto const width = sender.ActualWidth();
        auto const height = sender.ActualHeight();
        auto const color = ActualTheme() == winrt::Microsoft::UI::Xaml::ElementTheme::Light?
			winrt::Windows::UI::ColorHelper::FromArgb(32, 0, 0, 0) :
            winrt::Windows::UI::ColorHelper::FromArgb(32, 255, 255, 255);

        for (int i = 0; i < width; i += BackgroundCircleDistance)
        {
            for (int j = 0; j < height; j += BackgroundCircleDistance)
            {
                drawingSession.FillCircle(i, j, 1, color);
            }
        }
    }

    void SpeedGraph::CanvasControl_ActualThemeChanged(winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const&)
    {
		sender.as<winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl>().Invalidate();
    }

    void SpeedGraph::Pause()
    {
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Pause", false);
    }

    void SpeedGraph::Error()
    {
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Error", false);
    }

    void winrt::FastCopy::implementation::SpeedGraph::UserControl_SizeChanged(
        winrt::Windows::Foundation::IInspectable const&, 
        winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
        m_graphData.NewSize(e.NewSize());
    }

}
