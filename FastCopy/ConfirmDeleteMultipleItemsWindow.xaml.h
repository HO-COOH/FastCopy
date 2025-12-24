#pragma once

#include "ConfirmDeleteMultipleItemsWindow.g.h"
#include "ConfirmWindowBase.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteMultipleItemsWindow : ConfirmDeleteMultipleItemsWindowT<ConfirmDeleteMultipleItemsWindow>, ConfirmWindowBase<ConfirmDeleteMultipleItemsWindow>
    {
        ConfirmDeleteMultipleItemsWindow();

        winrt::hstring const& ItemCountText() const;
        
    private:
        winrt::hstring m_itemCountText;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteMultipleItemsWindow : ConfirmDeleteMultipleItemsWindowT<ConfirmDeleteMultipleItemsWindow, implementation::ConfirmDeleteMultipleItemsWindow>
    {
    };
}
