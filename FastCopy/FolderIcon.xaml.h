#pragma once

#include "FolderIcon.g.h"

namespace winrt::FastCopy::implementation
{
    struct FolderIcon : FolderIconT<FolderIcon>
    {
        FolderIcon() = default;

        winrt::hstring Path();
        winrt::fire_and_forget Path(winrt::hstring const& value);

        winrt::hstring m_path;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FolderIcon : FolderIconT<FolderIcon, implementation::FolderIcon>
    {
    };
}
