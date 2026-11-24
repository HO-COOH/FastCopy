#pragma once

#include "ConfirmDeleteMultipleItemsWindow.g.h"
#include "ConfirmWindowBase.h"

namespace winrt::FastCopy::implementation
{
    struct RobocopyViewModel;

    struct ConfirmDeleteMultipleItemsWindow : ConfirmDeleteMultipleItemsWindowT<ConfirmDeleteMultipleItemsWindow>, ConfirmWindowBase<ConfirmDeleteMultipleItemsWindow>
    {
        ConfirmDeleteMultipleItemsWindow(RobocopyViewModel& viewModel);

        winrt::hstring ItemCountText();
        
        constexpr static winrt::Windows::Graphics::SizeInt32 Size{ 545, 152 };
    private:
        winrt::hstring m_itemCountText;
    };
}
