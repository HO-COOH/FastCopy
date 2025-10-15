#pragma once

#include "FileInfoViewModel.g.h"
#include "PropertyChangeHelper.hpp"
#include "PathUtils.h"
#include "FileTime.h"

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel>
    {
        FileInfoViewModel(winrt::hstring path, bool isSource);

        winrt::hstring Path() { return m_path; }
        winrt::hstring Filename();

        winrt::Windows::Foundation::DateTime CreateTime() { return m_fileTime.CreateTime(); }
        winrt::Windows::Foundation::DateTime LastAccessTime() { return m_fileTime.LastAccessTime(); }
        winrt::Windows::Foundation::DateTime LastWriteTime() { return m_fileTime.LastWriteTime(); }
        
        uint64_t Bytes();
        bool Selected() { return m_selected; }
        void Selected(bool value);

        winrt::Microsoft::UI::Xaml::Media::ImageSource Bitmap();

        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> tooltipInfo();
    private:
        WinRTFileTime m_fileTime;
        winrt::hstring m_path;
        bool const m_isSource;
        bool m_selected = false;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel, implementation::FileInfoViewModel>
    {
    };
}
