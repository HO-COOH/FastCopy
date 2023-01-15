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
        switch (ViewModel().Mode())
        {
            case FastCopy::Mode::Copy: CopyModeItem_Click(nullptr, nullptr); break;
            case FastCopy::Mode::Move: MoveModeItem_Click(nullptr, nullptr); break;
            case FastCopy::Mode::Custom: CustomModeItem_Click(nullptr, nullptr); break;
            default: throw std::exception{ "Unknown xcopy mode" };
        }
    }

    winrt::FastCopy::XCopyViewModel MainPage::ViewModel()
    {
        return ViewModelLocator::GetInstance().XCopyViewModel();
    }
}


void winrt::FastCopy::implementation::MainPage::CopyModeItem_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    ViewModel().Mode(FastCopy::Mode::Copy);
    ModeButton().Content(winrt::box_value(L"Copy"));
}


void winrt::FastCopy::implementation::MainPage::CustomModeItem_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    ViewModel().Mode(FastCopy::Mode::Custom);
    ModeButton().Content(winrt::box_value(L"Custom"));
}


void winrt::FastCopy::implementation::MainPage::MoveModeItem_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
{
    ViewModel().Mode(FastCopy::Mode::Move);
    ModeButton().Content(winrt::box_value(L"Move"));
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
