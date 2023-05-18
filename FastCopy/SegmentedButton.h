#pragma once

#include "SegmentedButton.g.h"

namespace winrt::FastCopy::implementation
{
    struct SegmentedButton : SegmentedButtonT<SegmentedButton>
    {
        SegmentedButton() { InitializeComponent(); }

        winrt::Windows::Foundation::IInspectable Content1();
        void Content1(winrt::Windows::Foundation::IInspectable content1);
        static winrt::Microsoft::UI::Xaml::DependencyProperty Content1Property();

        winrt::Windows::Foundation::IInspectable Content2();
        void Content2(winrt::Windows::Foundation::IInspectable content2);
        static winrt::Microsoft::UI::Xaml::DependencyProperty Content2Property();

        void Button1_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Button2_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        int Selection();
        void Selection(int value);
        static winrt::Microsoft::UI::Xaml::DependencyProperty SelectionProperty() { return m_selectionProperty; }
    private:
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_content1;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_content2;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_selectionProperty;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SegmentedButton : SegmentedButtonT<SegmentedButton, implementation::SegmentedButton>
    {
    };
}
