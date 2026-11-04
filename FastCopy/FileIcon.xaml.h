#pragma once

#include "FileIcon.g.h"
#include <include/TemplateControlHelper.hpp>
#include "IconBase.hpp"

namespace winrt::FastCopy::implementation
{
	struct FileIcon : FileIconT<FileIcon>, TemplateControlHelper<FileIcon, false, false>, IconBase<FileIcon>
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
