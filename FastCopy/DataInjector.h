#pragma once

#include "DataInjector.g.h"

namespace winrt::FastCopy::implementation
{
    struct DataInjector : DataInjectorT<DataInjector>
    {
        DataInjector() = default;

        static Microsoft::UI::Xaml::DependencyProperty DataProperty();

        static winrt::Windows::Foundation::IInspectable GetData(Microsoft::UI::Xaml::DependencyObject target);
        static void SetData(Microsoft::UI::Xaml::DependencyObject target, winrt::Windows::Foundation::IInspectable data);

    private:
        static Microsoft::UI::Xaml::DependencyProperty s_dataProperty;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct DataInjector : DataInjectorT<DataInjector, implementation::DataInjector>
    {
    };
}
