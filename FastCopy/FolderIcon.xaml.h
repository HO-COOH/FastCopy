#pragma once

#include "FolderIcon.g.h"
#include "IconBase.hpp"

namespace winrt::FastCopy::implementation
{
    struct FolderIcon : FolderIconT<FolderIcon>, IconBase<FolderIcon>
    {
        static auto getStorageItemFromPath(winrt::hstring const& path)
        {
            return winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(path);
        }
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FolderIcon : FolderIconT<FolderIcon, implementation::FolderIcon>
    {
    };
}
