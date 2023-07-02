// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ProgressBarEx.g.h"

namespace winrt::FastCopy::implementation
{
    struct ProgressBarEx : ProgressBarExT<ProgressBarEx>
    {
        ProgressBarEx();

        static winrt::Microsoft::UI::Xaml::DependencyProperty ColorProperty()
        {
            return s_colorProperty;
        }

        static winrt::Microsoft::UI::Xaml::DependencyProperty PercentProperty()
        {
            return s_percentProperty;
        }

        winrt::Windows::UI::Color Color()
        {
            return winrt::unbox_value<winrt::Windows::UI::Color>(GetValue(s_colorProperty));
        }

        void Color(winrt::Windows::UI::Color value)
        {
            SetValue(s_colorProperty, winrt::box_value(value));
            Brush().Color(value);
        }

        double Percent()
        {
            return winrt::unbox_value<double>(GetValue(s_percentProperty));
        }

        void Percent(double value)
        {
            if (isnan(value) || value < 0 || value > 100)
                return;
            SetValue(s_percentProperty, winrt::box_value(value));
            RectColumn().Width(winrt::Microsoft::UI::Xaml::GridLength{ value, winrt::Microsoft::UI::Xaml::GridUnitType::Star });
            RestColumn().Width(winrt::Microsoft::UI::Xaml::GridLength{ 100 - value, winrt::Microsoft::UI::Xaml::GridUnitType::Star });
        }

        double Rest()
        {
            return 100 - Percent();
        }
    private:
        winrt::Microsoft::UI::Composition::PointLight m_pointLight{ nullptr };
        winrt::Microsoft::UI::Composition::AmbientLight m_ambientLight{ nullptr };
        winrt::Microsoft::UI::Composition::ScalarKeyFrameAnimation m_animation{ nullptr };
        winrt::Microsoft::UI::Composition::CompositionPropertySet m_propertySet{ nullptr };
        static winrt::Microsoft::UI::Xaml::DependencyProperty s_colorProperty;
        static winrt::Microsoft::UI::Xaml::DependencyProperty s_percentProperty;
    public:
        void UserControl_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Bar_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ProgressBarEx : ProgressBarExT<ProgressBarEx, implementation::ProgressBarEx>
    {
    };
}
