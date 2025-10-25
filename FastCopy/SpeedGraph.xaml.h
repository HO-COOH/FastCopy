// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.Brushes.h>
#include <winrt/Microsoft.Graphics.Canvas.Geometry.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>
#include "SpeedGraph.g.h"
#include <chrono>

namespace winrt::FastCopy::implementation
{
    struct SpeedGraph : SpeedGraphT<SpeedGraph>
    {
        SpeedGraph() = default;

        void SetSpeed(double percent, uint64_t speed);

        void Pause();
        void Error();
    private:
        double m_currentPercent{};
        uint64_t m_total{};
        uint64_t m_currentMax = 1024;

        winrt::Windows::Foundation::Size m_graphSize{};

        float getX(uint64_t progressBytes);
        float getY(uint64_t speed);
        
        void addInitialPointIfNeeded(uint32_t& count, winrt::Microsoft::UI::Xaml::Media::PointCollection& points);

        /**
         * @brief Recalculate graph point because of the speed scale changed
         */
        void resizeGraphPoint(float ratio);
        float m_ratio = 1.0f;


        /**
         * @brief Make animation when the scale is changed
         */
        void makeAnimation();

        void makeAnimation(float y);
        constexpr static auto BackgroundCircleDistance = 6;

        constexpr static winrt::Microsoft::UI::Xaml::Duration speedLineAndTextAnimationDuration
        {
            .TimeSpan = std::chrono::milliseconds{300},
            .Type = winrt::Microsoft::UI::Xaml::DurationType::TimeSpan
        };
    public:
        void CanvasControl_Draw(winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl const& sender, winrt::Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs const& args);
        void UserControl_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
        void CanvasControl_ActualThemeChanged(winrt::Microsoft::UI::Xaml::FrameworkElement const& sender, winrt::Windows::Foundation::IInspectable const& args);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SpeedGraph : SpeedGraphT<SpeedGraph, implementation::SpeedGraph>
    {
    };
}
