#pragma once

#include "FileInfoViewModel.g.h"
#include "PropertyChangeHelper.hpp"

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel>, PropertyChangeHelper<FileInfoViewModel>
    {
        FileInfoViewModel(winrt::hstring path) : m_path{ path } 
        {
        }

        winrt::hstring Filename();
        winrt::Windows::Foundation::DateTime CreationDate();
        uint64_t Bytes();
        bool Selected() { return m_selected; }
        void Selected(bool value);

        winrt::Microsoft::UI::Xaml::Media::ImageSource Bitmap();

        winrt::event_token SelectionChanged(winrt::Windows::Foundation::EventHandler<bool> const& handler) { return m_selectionChanged.add(handler); }
        void SelectionChanged(winrt::event_token const& token) { m_selectionChanged.remove(token); }
    private:

        winrt::event<Windows::Foundation::EventHandler<bool>> m_selectionChanged;
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
