#pragma once

#include "ConfirmDeleteFolderWindow.g.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow>
    {
        ConfirmDeleteFolderWindow() = default;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow, implementation::ConfirmDeleteFolderWindow>
    {
    };
}
