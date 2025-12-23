#pragma once

#include "ConfirmDeleteFileWindow.g.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow>
    {
        ConfirmDeleteFileWindow();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow, implementation::ConfirmDeleteFileWindow>
    {
    };
}
