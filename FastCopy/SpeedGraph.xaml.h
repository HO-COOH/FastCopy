// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "SpeedGraph.g.h"
#include <SpeedGraphData.h>

namespace winrt::Microsoft::Graphics::Canvas::UI::Xaml
{
    struct CanvasControl;
    struct CanvasDrawEventArgs;
}

namespace winrt::FastCopy::implementation
{
    struct SpeedGraph : SpeedGraphT<SpeedGraph>
    {
        constexpr double Percent() const noexcept { return m_percent; }
        constexpr void Percent(double value) noexcept { m_percent = value; }

        double Speed() const noexcept { return m_speed; }
        void Speed(double value);

        FastCopy::TaskbarState State() const noexcept { return m_state; }
        void State(FastCopy::TaskbarState value);

        winrt::Microsoft::UI::Xaml::Media::PointCollection Points();
    private:
        SpeedGraphData m_graphData;

        double m_percent{};
        double m_speed{};
        FastCopy::TaskbarState m_state{ FastCopy::TaskbarState::Indeterminate };
        bool m_hasData{};

        void setSpeed(double percent, uint64_t speed);

        /**
         * @brief Recalculate graph point because of the speed scale changed
         */
        void resizeGraphPoint(float ratio);

        /**
         * @brief Make animation when the scale is changed
         */
        void makeAnimation();

        constexpr static auto BackgroundCircleDistance = 6;
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
