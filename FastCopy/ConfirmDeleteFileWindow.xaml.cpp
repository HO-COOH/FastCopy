#include "pch.h"
#include "ConfirmDeleteFileWindow.xaml.h"
#if __has_include("ConfirmDeleteFileWindow.g.cpp")
#include "ConfirmDeleteFileWindow.g.cpp"
#endif

#include "ResourceHelper.h"
#include "ViewModelLocator.h"
#include "RobocopyViewModel.h"
#include <filesystem>

namespace winrt::FastCopy::implementation
{
    ConfirmDeleteFileWindow::ConfirmDeleteFileWindow()
    {
        Title(GetStringResource(L"ConfirmDeleteFileWindowTitle"));
        
        auto viewModel = winrt::get_self<RobocopyViewModel>(ViewModelLocator::GetInstance().RobocopyViewModel());
        auto& taskFile = *viewModel->m_recordFile;
        std::filesystem::path filePath{ *taskFile.begin() };
        m_fileName = winrt::hstring{ filePath.filename().wstring() };
    }

    winrt::hstring const& ConfirmDeleteFileWindow::FileName() const
    {
        return m_fileName;
    }
}
