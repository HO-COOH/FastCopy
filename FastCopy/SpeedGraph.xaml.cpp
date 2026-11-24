// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "SpeedGraph.xaml.h"
#if __has_include("SpeedGraph.g.cpp")
#include "SpeedGraph.g.cpp"
#endif
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
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

    void SpeedGraph::Speed(double value)
    {
        m_speed = value;
        setSpeed(m_percent * 100.0, static_cast<uint64_t>(value));
    }

    void SpeedGraph::State(FastCopy::TaskbarState value)
    {
        m_state = value;
        switch (value)
        {
            case FastCopy::TaskbarState::Paused:
                winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Pause", false);
                break;
            case FastCopy::TaskbarState::Error:
            case FastCopy::TaskbarState::Failed:
                winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Error", false);
                break;
            case FastCopy::TaskbarState::Normal:
                winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Normal", false);
                break;
            default:
                break;
        }
    }

    void SpeedGraph::setSpeed(double percent, uint64_t speed)
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

    void SpeedGraph::makeAnimation()
    {
        auto const p = m_graphData.GetLastPoint();
        auto const y = m_graphData.Height() - ((m_graphData.Height() - p.Y) * m_graphData.GetRatio());
        LineTranslateAnimation().To(y);
        TextTranslateAnimation().To(y);
        SpeedLineAndTextAnimation().Begin();
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

    void winrt::FastCopy::implementation::SpeedGraph::UserControl_SizeChanged(
        winrt::Windows::Foundation::IInspectable const&, 
        winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
        m_graphData.NewSize(e.NewSize());
    }

}
