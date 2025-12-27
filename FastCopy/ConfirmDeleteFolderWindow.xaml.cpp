#include "pch.h"
#include "ConfirmDeleteFolderWindow.xaml.h"
#if __has_include("ConfirmDeleteFolderWindow.g.cpp")
#include "ConfirmDeleteFolderWindow.g.cpp"
#endif

#include "ResourceHelper.h"
#include "ViewModelLocator.h"
#include "RobocopyViewModel.h"
#include <filesystem>
#include "PathUtils.h"

namespace winrt::FastCopy::implementation
{
    ConfirmDeleteFolderWindow::ConfirmDeleteFolderWindow()
    {
        Title(GetStringResource(L"ConfirmDeleteFolderWindowTitle"));
        
        auto viewModel = winrt::get_self<RobocopyViewModel>(ViewModelLocator::GetInstance().RobocopyViewModel());
        auto& taskFile = *viewModel->m_recordFile;
        std::filesystem::path folderPath{ *taskFile.begin() };
        m_folderPath = winrt::hstring{ ToBackslash(folderPath.wstring()) };
        m_folderName = winrt::hstring{ folderPath.filename().wstring() };
        
        WIN32_FILE_ATTRIBUTE_DATA fileAttrData{};
        if (GetFileAttributesExW(m_folderPath.c_str(), GetFileExInfoStandard, &fileAttrData))
        {
            m_dateCreated = winrt::clock::from_FILETIME(fileAttrData.ftCreationTime);
        }
    }

    winrt::hstring ConfirmDeleteFolderWindow::FolderName()
    {
        return m_folderName;
    }

    winrt::hstring ConfirmDeleteFolderWindow::FolderPath()
    {
        return m_folderPath;
    }

    winrt::Windows::Foundation::DateTime ConfirmDeleteFolderWindow::DateCreated()
    {
        return m_dateCreated;
    }
}

