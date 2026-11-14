#pragma once

#include "FileIcon.g.h"
#include "IconBase.hpp"

namespace winrt::FastCopy::implementation
{
	struct FileIcon : FileIconT<FileIcon>, IconBase<FileIcon>
    {
        static auto getStorageItemFromPath(winrt::hstring const& path)
        {
            return winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path);
        }
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FileIcon : FileIconT<FileIcon, implementation::FileIcon>
    {
    };
}
