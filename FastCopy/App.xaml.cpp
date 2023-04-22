// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"

#include "App.xaml.h"
#include "CopyDialogWindow.xaml.h"


using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace FastCopy;
using namespace FastCopy::implementation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
}

#include "ViewModelLocator.h"
#include "FastCopyCommandParser.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
winrt::Windows::Foundation::IAsyncAction GetFromClipboard()
{
    //get data from copy paste
    auto packageView = winrt::Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
    if (!packageView.Contains(winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats::StorageItems()))
        co_return;

    auto sources = ViewModelLocator::GetInstance().XCopyViewModel().Sources();
    auto storageItems = co_await packageView.GetStorageItemsAsync();
    for (auto storageItem : storageItems)
    {
        if (storageItem.IsOfType(winrt::Windows::Storage::StorageItemTypes::File))
        {
            sources.Append(winrt::FastCopy::ExplorerItem{ storageItem.as<winrt::Windows::Storage::StorageFile>() });
        }
        else if (storageItem.IsOfType(winrt::Windows::Storage::StorageItemTypes::Folder))
        {
            sources.Append(winrt::FastCopy::ExplorerItem{ storageItem.as<winrt::Windows::Storage::StorageFolder>(), true, 3 });
        }
    }
}

#include "CommandLine.h"
void App::OnLaunched(LaunchActivatedEventArgs const&)
{
    auto const recordFile = Command::Get().RecordFile();
    //MessageBox(NULL, recordFile.data(), L"", 0);
    ViewModelLocator::GetInstance().RobocopyViewModel().Destination(Command::Get().GetDestination());
    ViewModelLocator::GetInstance().RobocopyViewModel().RecordFile(recordFile);

    window = make<CopyDialogWindow>();
    window.Activate();
    m_helper.emplace(window);
}
