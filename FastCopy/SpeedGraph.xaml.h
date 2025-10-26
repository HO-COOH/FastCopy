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
#include <SpeedGraphData.h>
#include <ViewModelLocator.h>

namespace winrt::FastCopy::implementation
{
    struct SpeedGraph : SpeedGraphT<SpeedGraph>
    {
        SpeedGraph() = default;

        void SetSpeed(double percent, uint64_t speed);

        void Pause();
        void Error();

        winrt::Microsoft::UI::Xaml::Media::PointCollection Points();
    private:
        SpeedGraphData& m_graphData{ ViewModelLocator::GetInstance().SpeedGraphData() };

        /**
         * @brief Recalculate graph point because of the speed scale changed
         */
        void resizeGraphPoint(float ratio);



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
