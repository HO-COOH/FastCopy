#include "pch.h"
#include "SegmentedButton.h"
#if __has_include("SegmentedButton.g.cpp")
#include "SegmentedButton.g.cpp"
#endif


namespace winrt::FastCopy::implementation
{
    winrt::Microsoft::UI::Xaml::DependencyProperty SegmentedButton::m_content1 = 
        winrt::Microsoft::UI::Xaml::DependencyProperty::Register(
            L"Content1",
            winrt::xaml_typename<winrt::Windows::Foundation::IInspectable>(),
            winrt::xaml_typename<winrt::FastCopy::SegmentedButton>(),
            {nullptr}
        );

    winrt::Microsoft::UI::Xaml::DependencyProperty SegmentedButton::m_content2 =
        winrt::Microsoft::UI::Xaml::DependencyProperty::Register(
            L"Content2",
            winrt::xaml_typename<winrt::Windows::Foundation::IInspectable>(),
            winrt::xaml_typename<winrt::FastCopy::SegmentedButton>(),
            { nullptr }
        );

    winrt::Microsoft::UI::Xaml::DependencyProperty SegmentedButton::m_selectionProperty =
        winrt::Microsoft::UI::Xaml::DependencyProperty::Register(
            L"Selection",
            winrt::xaml_typename<int>(),
            winrt::xaml_typename<winrt::FastCopy::SegmentedButton>(),
            { nullptr }
    );
    winrt::Windows::Foundation::IInspectable SegmentedButton::Content1()
    {
        return GetValue(m_content1);
    }
    winrt::Windows::Foundation::IInspectable SegmentedButton::Content2()
    {
        return GetValue(m_content2);
    }
    void SegmentedButton::Content2(winrt::Windows::Foundation::IInspectable content2)
    {
        SetValue(m_content2, content2);
    }
    void SegmentedButton::Content1(winrt::Windows::Foundation::IInspectable content1)
    {
        SetValue(m_content1, content1);
    }
    winrt::Microsoft::UI::Xaml::DependencyProperty SegmentedButton::Content2Property()
    {
        return m_content2;
    }
    winrt::Microsoft::UI::Xaml::DependencyProperty SegmentedButton::Content1Property()
    {
        return m_content1;
    }


    void SegmentedButton::Button1_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        Button2().IsChecked(false);
    }


    void SegmentedButton::Button2_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        Button1().IsChecked(false);
    }
    int SegmentedButton::Selection()
    {
        return 0;
    }
    void SegmentedButton::Selection(int value)
    {
        if (value == 0)
        {
            Button1().IsChecked(true);
            Button2().IsChecked(false);
        }
        else if(value == 1)
        {
            Button1().IsChecked(false);
            Button2().IsChecked(true);
        }
    }
}
