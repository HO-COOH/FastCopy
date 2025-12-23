#pragma once

#include "ConfirmDeleteMultipleItemsWindow.g.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteMultipleItemsWindow : ConfirmDeleteMultipleItemsWindowT<ConfirmDeleteMultipleItemsWindow>
    {
        ConfirmDeleteMultipleItemsWindow()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteMultipleItemsWindow : ConfirmDeleteMultipleItemsWindowT<ConfirmDeleteMultipleItemsWindow, implementation::ConfirmDeleteMultipleItemsWindow>
    {
    };
}
