#pragma once

#include "AnimatedValue.g.h"

namespace winrt::FastCopy::implementation
{
    struct AnimatedValue : AnimatedValueT<AnimatedValue>
    {
        AnimatedValue()
        {
            OutputDebugString(L"Constructed");
        }

        static winrt::Microsoft::UI::Xaml::DependencyProperty ValueProperty()
        {
            return m_valueProperty;
        }

        double Value();
        void Value(double value);
    private:
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_valueProperty;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct AnimatedValue : AnimatedValueT<AnimatedValue, implementation::AnimatedValue>
    {
    };
}
