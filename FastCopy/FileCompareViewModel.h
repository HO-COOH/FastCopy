#pragma once

#include "FileCompareViewModel.g.h"
#include "FileInfoViewModel.g.h"

namespace winrt::FastCopy::implementation
{
    struct FileCompareViewModel : FileCompareViewModelT<FileCompareViewModel>
    {
        FileCompareViewModel(winrt::hstring path1, winrt::hstring path2);
        
        winrt::FastCopy::FileInfoViewModel File1() { return m_file1; }
        winrt::FastCopy::FileInfoViewModel File2() { return m_file2; }
    private:
        void setRobocopyViewModelDuplicateSelection(bool isFile1, bool isSelected);
        winrt::FastCopy::FileInfoViewModel m_file1;
        winrt::FastCopy::FileInfoViewModel m_file2;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FileCompareViewModel : FileCompareViewModelT<FileCompareViewModel, implementation::FileCompareViewModel>
    {
    };
}
