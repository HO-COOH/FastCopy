#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
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
    private:
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_content1;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_content2;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SegmentedButton : SegmentedButtonT<SegmentedButton, implementation::SegmentedButton>
    {
    };
}
