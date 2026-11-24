#pragma once

#include "FileCompareViewModel.g.h"

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel;
    struct RobocopyViewModel;

    struct FileCompareViewModel : FileCompareViewModelT<FileCompareViewModel>
    {
        FileCompareViewModel(
            std::wstring_view path1, 
            std::wstring_view path2,
            RobocopyViewModel& robocopyViewModel
        );
        ~FileCompareViewModel();
        
        winrt::FastCopy::FileInfoViewModel File1();
        winrt::FastCopy::FileInfoViewModel File2();
    private:
        winrt::com_ptr<FileInfoViewModel> m_file1;
        winrt::com_ptr<FileInfoViewModel> m_file2;
    };
}
