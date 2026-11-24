#include "pch.h"
#include "ConfirmDeleteFileWindow.xaml.h"
#if __has_include("ConfirmDeleteFileWindow.g.cpp")
#include "ConfirmDeleteFileWindow.g.cpp"
#endif

#include "ResourceHelper.h"
#include "RobocopyViewModel.h"
#include "PathUtils.h"
#include <wil/com.h>
#include "PropertyDescription.h"
#include <propkey.h>
#include "RecycleBinIcon.h"
#include "FileInfoViewModel.h"

namespace winrt::FastCopy::implementation
{
    ConfirmDeleteFileWindow::ConfirmDeleteFileWindow(RobocopyViewModel& viewModel)
    {
        auto& taskFile = viewModel.m_recordFile;
        m_path = std::filesystem::path{ taskFile.GetItemPath(0) };
        m_fileInfo = winrt::make_self<FileInfoViewModel>(m_path.wstring(), false, viewModel);
    }

    ConfirmDeleteFileWindow::~ConfirmDeleteFileWindow() = default;

    winrt::hstring ConfirmDeleteFileWindow::FilePath()
    {
        return winrt::hstring{ ToBackslash(m_path.wstring()) };
    }
    winrt::FastCopy::FileInfoViewModel ConfirmDeleteFileWindow::FileInfo()
    {
        return *m_fileInfo;
    }

    winrt::hstring ConfirmDeleteFileWindow::TypeDescription()
    {
        return PropertyDescription{ PKEY_FileDescription }.GetDisplayName().get();
    }

    winrt::hstring ConfirmDeleteFileWindow::SizeDescription()
    {
        return PropertyDescription{ PKEY_Size }.GetDisplayName().get();
    }

    winrt::hstring ConfirmDeleteFileWindow::DateDescription()
    {
        return PropertyDescription{ PKEY_DateModified }.GetDisplayName().get();
    }
}
