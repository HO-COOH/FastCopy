#include "pch.h"
#include "ConfirmDeleteMultipleItemsWindow.xaml.h"
#if __has_include("ConfirmDeleteMultipleItemsWindow.g.cpp")
#include "ConfirmDeleteMultipleItemsWindow.g.cpp"
#endif

#include "ResourceHelper.h"
#include "RobocopyViewModel.h"

namespace winrt::FastCopy::implementation
{
    ConfirmDeleteMultipleItemsWindow::ConfirmDeleteMultipleItemsWindow(RobocopyViewModel& viewModel)
    {
        auto& taskFile = viewModel.m_recordFile;
        m_itemCountText = winrt::to_hstring(taskFile.GetItemCount());
    }

    winrt::hstring ConfirmDeleteMultipleItemsWindow::ItemCountText()
    {
        return m_itemCountText;
    }
}
