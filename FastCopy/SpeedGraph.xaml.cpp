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
    SpeedGraph::SpeedGraph()
    {
        InitializeComponent();
    }

    void SpeedGraph::AddPoint(uint64_t progressBytes, uint64_t speed)
    {
        auto const x = getX(progressBytes);

        auto count = Shape().Points().Size();
        if (count == 0)
        {
            Shape().Points().Append({ 0, (float)ActualHeight() });
            ++count;
        }

        if (m_currentMax == 0)
            m_currentMax = speed;

        if (count == 1)
        {
            Shape().Points().Append({ x, getY(speed) });
        }
        else
        {
            if (count == 2)
                Shape().Points().Append({ x, getY(speed) });
            else
            {
                Shape().Points().SetAt(Shape().Points().Size() - 1, { x, getY(speed) });
            }
            Shape().Points().Append({ x, m_graphSize.Height });
        }
        //OutputDebugString(std::format(L"{}\n", speed).data());
        if (m_currentMax < speed)
        {
            //OutputDebugString(std::format(L"max: {} -> {}\n", m_currentMax, speed).data());
            resizeGraphPoint(static_cast<float>(m_currentMax) / speed);
            m_currentMax = speed;
        }

        if (m_start == decltype(m_start){})
            m_start = std::chrono::steady_clock::now();

        if (count > 2)
            makeAnimation();
    }

    void SpeedGraph::SetSpeed(uint64_t speed)
    {
        //too large, maybe nan, we just return
        if (speed >= 1024ull * 1024 * 1024 * 1024)
            return;

        if (m_currentMax < speed)
        {
            resizeGraphPoint(static_cast<float>(m_currentMax) / speed);
            m_currentMax = speed;
        }
        makeAnimation(m_graphSize.Height * (1.0f - static_cast<float>(speed) / m_currentMax));
    }

    void SpeedGraph::SetSpeed(double percent, uint64_t speed)
    {
        float const x = percent / 100.0 * m_graphSize.Width;

        auto count = Shape().Points().Size();
        if (count == 0)
        {
            Shape().Points().Append({ 0, (float)ActualHeight() });
            ++count;
        }

        if (m_currentMax == 0)
            m_currentMax = speed;

        if (count == 1)
        {
            Shape().Points().Append({ x, getY(speed) });
        }
        else
        {
            if (count == 2)
                Shape().Points().Append({ x, getY(speed) });
            else
            {
                Shape().Points().SetAt(Shape().Points().Size() - 1, { x, getY(speed) });
            }
            Shape().Points().Append({ x, m_graphSize.Height });
        }
        //OutputDebugString(std::format(L"{}\n", speed).data());
        if (m_currentMax < speed)
        {
            //OutputDebugString(std::format(L"max: {} -> {}\n", m_currentMax, speed).data());
            resizeGraphPoint(static_cast<float>(m_currentMax) / speed);
            m_currentMax = speed;
        }

        if (m_start == decltype(m_start){})
            m_start = std::chrono::steady_clock::now();

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

    void SpeedGraph::resizeGraphPoint(float ratio)
    {
        auto const m_ratioCopy = m_ratio;
        m_ratio *= ratio;
        ScaleDownAnimation().To(m_ratio);
        ScaleDown().Begin();
    }

    void SpeedGraph::makeAnimation(float y)
    {
        constexpr winrt::Windows::Foundation::TimeSpan duration{std::chrono::milliseconds{300}};

        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard s;
        winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation lineTranslate;
        winrt::Microsoft::UI::Xaml::Media::Animation::DoubleAnimation textTranslate;

        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(lineTranslate, LineTranslate());
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(lineTranslate, L"Y");
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTarget(textTranslate, TextTranslate());
        winrt::Microsoft::UI::Xaml::Media::Animation::Storyboard::SetTargetProperty(textTranslate, L"Y");

        lineTranslate.Duration({ duration, winrt::Microsoft::UI::Xaml::DurationType::TimeSpan });
        textTranslate.Duration({ duration, winrt::Microsoft::UI::Xaml::DurationType::TimeSpan });

        winrt::Microsoft::UI::Xaml::Media::Animation::ExponentialEase ease;
        ease.Exponent(7);
        ease.EasingMode(winrt::Microsoft::UI::Xaml::Media::Animation::EasingMode::EaseOut);
        lineTranslate.EasingFunction(ease);
        textTranslate.EasingFunction(ease);


        lineTranslate.To(y);
        textTranslate.To(y);
        s.Children().Append(lineTranslate);
        s.Children().Append(textTranslate);
        s.Begin();
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
        auto const color = winrt::Windows::UI::ColorHelper::FromArgb(32, 255, 255, 255);

        for (int i = 0; i < width; i += BackgroundCircleDistance)
        {
            for (int j = 0; j < height; j += BackgroundCircleDistance)
            {
                drawingSession.FillCircle(i, j, 1, color);
            }
        }
    }

    void SpeedGraph::UserControl_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
    {
        auto const newSize = e.NewSize();
        if (m_graphSize != newSize && newSize.Width != 0 && newSize.Height != 0)
        {
            m_graphSize = e.NewSize();

            /*Set new x-value for existing points to scale */
            auto points = Shape().Points();
            auto const pointsCount = points.Size();
            for (int i = 0; i < pointsCount; ++i)
            {

            }
        }
    }
    void SpeedGraph::Pause()
    {
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Pause", false);
    }

    void SpeedGraph::Error()
    {
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"Error", false);
    }
}
