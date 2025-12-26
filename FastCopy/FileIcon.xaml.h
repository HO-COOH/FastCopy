#pragma once

#include "FileIcon.g.h"
#include <include/TemplateControlHelper.hpp>

namespace winrt::FastCopy::implementation
{
    struct FileIcon : FileIconT<FileIcon>, TemplateControlHelper<FileIcon>
    {
        FileIcon() = default;

        winrt::hstring Path();
        winrt::fire_and_forget Path(winrt::hstring const& value);

        winrt::hstring m_path;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FileIcon : FileIconT<FileIcon, implementation::FileIcon>
    {
    };
}
