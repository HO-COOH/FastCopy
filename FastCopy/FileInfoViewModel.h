#pragma once

#include "FileInfoViewModel.g.h"
#include "PropertyChangeHelper.hpp"

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel>, PropertyChangeHelper<FileInfoViewModel>
    {
        FileInfoViewModel(winrt::hstring path) : m_path{ path } { }

        winrt::hstring Filename();
        winrt::Windows::Foundation::DateTime CreationDate();
        uint64_t Bytes();
        bool Selected() { return m_selected; }
        void Selected(bool value);

        winrt::Microsoft::UI::Xaml::Media::ImageSource Bitmap();

    private:
        winrt::hstring m_path;
        bool m_selected = false;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel, implementation::FileInfoViewModel>
    {
    };
}
