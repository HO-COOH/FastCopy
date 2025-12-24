#include "pch.h"
#include "ConfirmDeleteFolderWindow.xaml.h"
#if __has_include("ConfirmDeleteFolderWindow.g.cpp")
#include "ConfirmDeleteFolderWindow.g.cpp"
#endif

#include "ResourceHelper.h"
#include "ViewModelLocator.h"
#include "RobocopyViewModel.h"
#include <filesystem>

namespace winrt::FastCopy::implementation
{
    ConfirmDeleteFolderWindow::ConfirmDeleteFolderWindow()
    {
        Title(GetStringResource(L"ConfirmDeleteFolderWindowTitle"));
        
        auto viewModel = winrt::get_self<RobocopyViewModel>(ViewModelLocator::GetInstance().RobocopyViewModel());
        auto& taskFile = *viewModel->m_recordFile;
        std::filesystem::path folderPath{ *taskFile.begin() };
        m_folderName = winrt::hstring{ folderPath.filename().wstring() };
    }

    winrt::hstring const& ConfirmDeleteFolderWindow::FolderName() const
    {
        return m_folderName;
    }
}
