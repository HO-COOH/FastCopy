#pragma once

#include "SettingContainer.g.h"

namespace winrt::FastCopy::implementation
{
    struct SettingContainer : SettingContainerT<SettingContainer>
    {
        SettingContainer() = default;

        winrt::Windows::Foundation::IInspectable Symbol();
        void Symbol(winrt::Windows::Foundation::IInspectable symbol);
        static winrt::Microsoft::UI::Xaml::DependencyProperty SymbolProperty();

        winrt::hstring Title();
        void Title(winrt::hstring title);
        static winrt::Microsoft::UI::Xaml::DependencyProperty TitleProperty();

        winrt::hstring HelpText();
        void HelpText(winrt::hstring helpText);
        static winrt::Microsoft::UI::Xaml::DependencyProperty HelpTextProperty();

        winrt::Microsoft::UI::Xaml::Visibility ShowReset();
        void ShowReset(winrt::Microsoft::UI::Xaml::Visibility showReset);
        static winrt::Microsoft::UI::Xaml::DependencyProperty ShowResetProperty();

        winrt::Windows::Foundation::IInspectable HeaderContent();
        void HeaderContent(winrt::Windows::Foundation::IInspectable header);
        static winrt::Microsoft::UI::Xaml::DependencyProperty HeaderContentProperty();

        winrt::Windows::Foundation::IInspectable ExpanderContent();
        void ExpanderContent(winrt::Windows::Foundation::IInspectable expanderContent);
        static winrt::Microsoft::UI::Xaml::DependencyProperty ExpanderContentProperty();
    private:
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_symbolProperty;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_titleProperty;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_helpTextProperty;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_showResetProperty;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_headerContentProperty;
        static winrt::Microsoft::UI::Xaml::DependencyProperty m_expanderContentProperty;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingContainer : SettingContainerT<SettingContainer, implementation::SettingContainer>
    {
    };
}
