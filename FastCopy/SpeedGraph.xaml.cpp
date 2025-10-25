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
    void SpeedGraph::SetSpeed(double percent, uint64_t speed)
    {
        if (m_currentMax == 0 && speed == 0)
            return;

        //If we don't have progress on this new data point, don't show it on the graph
        if (percent <= m_currentPercent)
            return;

		m_currentPercent = percent;

		auto points = Shape().Points();

        auto count = points.Size();
        float const x = percent / 100.0 * m_graphSize.Width;
        auto const y = getY(speed);

        assert(count != 1);
        switch (count)
        {
            case 0:
                addInitialPointIfNeeded(count, points);
                // add first real points
				points.Append({ x, y });
                break;
            case 2:
                // now we can show the graph by adding this data point and the bottom point of the graph so that the polygon end at the axis
				points.Append({ x, y });
                points.Append({ x, m_graphSize.Height });
				break;
            default:
				// we modify last point (which is the bottom point of the graph) to the new data point, and add a new bottom point
				points.SetAt(points.Size() - 1, { x, y });
				points.Append({ x, m_graphSize.Height });
                break;
        }

      
        if (m_currentMax < speed)
        {
            resizeGraphPoint(static_cast<float>(m_currentMax) / speed);
            m_currentMax = speed;
        }

        if (count > 2)
        {
            AverageSpeedText().Text(ReadableUnitConverter::Speed::ToString<wchar_t>(speed).data());
            makeAnimation();
        }
    }

    float SpeedGraph::getX(uint64_t progressBytes)
    {
        return m_graphSize.Width * (static_cast<float>(progressBytes) / m_total);
    }

    float SpeedGraph::getY(uint64_t speed)
    {
        return m_graphSize.Height * (1.0f - static_cast<float>(speed) / m_currentMax / m_ratio);
    }

    void SpeedGraph::addInitialPointIfNeeded(uint32_t& count, winrt::Microsoft::UI::Xaml::Media::PointCollection& points)
    {
        if (count == 0)
        {
            points.Append({ 0, m_graphSize.Height });
            ++count;
        }
    }

    void SpeedGraph::resizeGraphPoint(float ratio)
    {
        m_ratio *= ratio;
        ScaleDownAnimation().To(m_ratio);
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
        auto const p = Shape().Points().GetAt(Shape().Points().Size() - 2);
        auto const y = m_graphSize.Height - ((m_graphSize.Height - p.Y) * m_ratio);
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

    void winrt::FastCopy::implementation::SpeedGraph::UserControl_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
		m_graphSize = e.NewSize();
    }

}
