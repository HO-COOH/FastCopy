#include "pch.h"
#include "AnimatedValue.h"
#if __has_include("AnimatedValue.g.cpp")
#include "AnimatedValue.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
    
    winrt::Microsoft::UI::Xaml::DependencyProperty AnimatedValue::m_valueProperty =
        winrt::Microsoft::UI::Xaml::DependencyProperty::Register(
            L"Value",
            winrt::xaml_typename<double>(),
            winrt::xaml_typename<winrt::FastCopy::AnimatedValue>(),
            winrt::Microsoft::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0) }
    );

    double AnimatedValue::Value()
    {
        return winrt::unbox_value<double>(GetValue(m_valueProperty));
    }
    void AnimatedValue::Value(double value)
    {
        OutputDebugString((std::to_wstring(value) + L"\n").data());
        SetValue(m_valueProperty, winrt::box_value(value));
    }
}
