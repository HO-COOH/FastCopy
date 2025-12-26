#pragma once
#include "FileInfoViewModel.g.h"
#include "WinRTFileTime.h"
#include <include/PropertyChangeHelper.hpp>
#include "ShellItem2.h"

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel>, WinRTFileTime, MvvmHelper::PropertyChangeHelper<FileInfoViewModel>
    {
        FileInfoViewModel(winrt::hstring path, bool isSource);

        winrt::hstring Path() { return m_path; }
        winrt::hstring Filename();

        winrt::hstring Description();
        
        uint64_t Bytes();
        bool Selected() { return m_selected; }
        void Selected(bool value);

        winrt::Microsoft::UI::Xaml::Media::ImageSource Bitmap();

        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> tooltipInfo();
    private:
        ShellItem2 m_shellItem;
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
