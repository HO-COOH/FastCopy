#pragma once

#include "FileInfoViewModel.g.h"
#include "PropertyChangeHelper.hpp"
#include "PathUtils.h"
#include "FileTime.h"

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel : FileInfoViewModelT<FileInfoViewModel>, PropertyChangeHelper<FileInfoViewModel>
    {
        FileInfoViewModel(winrt::hstring path) : m_fileTime{ path }, m_path { ToBackslash(path) }
        {
        }

        winrt::hstring Path() { return m_path; }
        winrt::hstring Filename();

        winrt::Windows::Foundation::DateTime CreateTime() { return m_fileTime.CreateTime(); }
        winrt::Windows::Foundation::DateTime LastAccessTime() { return m_fileTime.LastAccessTime(); }
        winrt::Windows::Foundation::DateTime LastWriteTime() { return m_fileTime.LastWriteTime(); }
        
        uint64_t Bytes();
        bool Selected() { return m_selected; }
        void Selected(bool value);

        winrt::Microsoft::UI::Xaml::Media::ImageSource Bitmap();

        winrt::event_token SelectionChanged(winrt::Windows::Foundation::EventHandler<bool> const& handler) { return m_selectionChanged.add(handler); }
        void SelectionChanged(winrt::event_token const& token) { m_selectionChanged.remove(token); }

        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> tooltipInfo();
    private:
        WinRTFileTime m_fileTime;
        std::vector<winrt::Windows::Foundation::IInspectable> m_tooltipInfo;
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
