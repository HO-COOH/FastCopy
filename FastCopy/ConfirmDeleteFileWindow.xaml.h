#pragma once

#include "ConfirmDeleteFileWindow.g.h"
#include "ConfirmWindowBase.h"
#include <filesystem>

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow>, ConfirmWindowBase<ConfirmDeleteFileWindow>
    {
        ConfirmDeleteFileWindow();

        winrt::hstring FilePath();
        
        winrt::FastCopy::FileInfoViewModel FileInfo();

        static winrt::hstring TypeDescription();
        static winrt::hstring SizeDescription();
        static winrt::hstring DateDescription();

        constexpr static winrt::Windows::Graphics::SizeInt32 Size{ 545, 222 };
    private:
        std::filesystem::path m_path;
        winrt::FastCopy::FileInfoViewModel m_fileInfo{ nullptr };
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow, implementation::ConfirmDeleteFileWindow>
    {
    };
}
