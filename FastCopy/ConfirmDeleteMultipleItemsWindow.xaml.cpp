#include "pch.h"
#include "ConfirmDeleteMultipleItemsWindow.xaml.h"
#if __has_include("ConfirmDeleteMultipleItemsWindow.g.cpp")
#include "ConfirmDeleteMultipleItemsWindow.g.cpp"
#endif

#include "ResourceHelper.h"
#include "ViewModelLocator.h"
#include "RobocopyViewModel.h"

namespace winrt::FastCopy::implementation
{
    ConfirmDeleteMultipleItemsWindow::ConfirmDeleteMultipleItemsWindow()
    {
        Title(GetStringResource(L"ConfirmDeleteMultipleItemsWindowTitle"));
        
        auto viewModel = winrt::get_self<RobocopyViewModel>(ViewModelLocator::GetInstance().RobocopyViewModel());
        auto& taskFile = *viewModel->m_recordFile;
        auto const itemCount = std::distance(taskFile.begin(), taskFile.end());
        m_itemCountText = winrt::hstring{ std::to_wstring(itemCount) + L" items" };
    }

    winrt::hstring const& ConfirmDeleteMultipleItemsWindow::ItemCountText() const
    {
        return m_itemCountText;
    }
}
