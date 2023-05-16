// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <ShObjIdl_core.h>

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    winrt::FastCopy::XCopyViewModel MainPage::ViewModel()
    {
        return ViewModelLocator::GetInstance().XCopyViewModel();
    }
}


winrt::Windows::Foundation::IAsyncAction winrt::FastCopy::implementation::MainPage::ChangeDestination()
{
    winrt::Windows::Storage::Pickers::FolderPicker folderPicker;
    folderPicker.FileTypeFilter().Append(L"*");
    folderPicker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::ComputerFolder);
    folderPicker.as<IInitializeWithWindow>()->Initialize(GetActiveWindow());

    auto folder = co_await folderPicker.PickSingleFolderAsync();
    if (folder == nullptr)
        co_return;

    ViewModel().Destination(folder.Path());
}


winrt::Windows::Foundation::IAsyncAction winrt::FastCopy::implementation::MainPage::AddFile()
{
    winrt::Windows::Storage::Pickers::FileOpenPicker filePicker;
    filePicker.FileTypeFilter().Append(L"*");
    filePicker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::ComputerFolder);
    filePicker.as<IInitializeWithWindow>()->Initialize(GetActiveWindow());
    auto files = co_await filePicker.PickMultipleFilesAsync();
    for (auto const& file : files)
    {
        ViewModel().Sources().Append(winrt::FastCopy::ExplorerItem{ file.Path(), winrt::FastCopy::ExplorerItemType::File });
    }
}


winrt::Windows::Foundation::IAsyncAction winrt::FastCopy::implementation::MainPage::AddFolder()
{
    winrt::Windows::Storage::Pickers::FolderPicker folderPicker;
    folderPicker.FileTypeFilter().Append(L"*");
    folderPicker.SuggestedStartLocation(winrt::Windows::Storage::Pickers::PickerLocationId::ComputerFolder);
    folderPicker.as<IInitializeWithWindow>()->Initialize(GetActiveWindow());

    auto folder = co_await folderPicker.PickSingleFolderAsync();
    if (folder == nullptr)
        co_return;

    ViewModel().Sources().Append(winrt::FastCopy::ExplorerItem{folder, true, 3});
}

void winrt::FastCopy::implementation::MainPage::RemoveItemMenuItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    auto sources = ViewModel().Sources();
    uint32_t index{};
    if (sources.IndexOf(sender.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().DataContext().as<winrt::FastCopy::ExplorerItem>(), index))
        sources.RemoveAt(index);
}


void winrt::FastCopy::implementation::MainPage::RenameMenuItem_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{

}


winrt::Windows::Foundation::IAsyncAction winrt::FastCopy::implementation::MainPage::MenuFlyoutItem_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    co_await AddFile();
}


winrt::Windows::Foundation::IAsyncAction winrt::FastCopy::implementation::MainPage::MenuFlyoutItem_Click_1(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    co_await AddFolder();
}


winrt::Windows::Foundation::IAsyncAction winrt::FastCopy::implementation::MainPage::ChangeDestinationButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    co_await ChangeDestination();
}

#include "CopyDialog.g.h"
void winrt::FastCopy::implementation::MainPage::ExecuteButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    winrt::Microsoft::UI::Xaml::Window dialogWindow;
    dialogWindow.Content(winrt::FastCopy::CopyDialog{});
    dialogWindow.Activate();
    ViewModel().Execute();
}
