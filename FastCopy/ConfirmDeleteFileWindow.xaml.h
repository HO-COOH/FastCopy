#pragma once

#include "ConfirmDeleteFileWindow.g.h"
#include "ConfirmWindowBase.h"
#include <filesystem>

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel;
    struct RobocopyViewModel;

    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow>, ConfirmWindowBase<ConfirmDeleteFileWindow>
    {
        ConfirmDeleteFileWindow(RobocopyViewModel& viewModel);
        ~ConfirmDeleteFileWindow();

        winrt::hstring FilePath();
        
        winrt::FastCopy::FileInfoViewModel FileInfo();

        static winrt::hstring TypeDescription();
        static winrt::hstring SizeDescription();
        static winrt::hstring DateDescription();

        constexpr static winrt::Windows::Graphics::SizeInt32 Size{ 545, 222 };
    private:
        std::filesystem::path m_path;
        winrt::com_ptr<FileInfoViewModel> m_fileInfo;
    };
}
namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow, implementation::ConfirmDeleteFileWindow>
    {
    };
}
