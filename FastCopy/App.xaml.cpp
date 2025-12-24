// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"

#include "App.xaml.h"
#include "CopyDialogWindow.xaml.h"
#include "SettingsWindow.xaml.h"
#include "WelcomeWindow.xaml.h"
#include "ConfirmDeleteFileWindow.xaml.h"
#include "ConfirmDeleteFolderWindow.xaml.h"
#include "ConfirmDeleteMultipleItemsWindow.xaml.h"
#include "ViewModelLocator.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "CommandLine.h"
#include "DebugLogger.h"
#include <winrt/Windows.System.h>
#include <filesystem>
#include "Global.h"
#include "RobocopyViewModel.h"
#include "CommandLineHandler.h"
#include "Settings.h"


namespace winrt::FastCopy::implementation
{
    namespace
    {
        template<typename ImplementationType>
        winrt::Windows::Foundation::IAsyncOperation<bool> ShowConfirmWindowAsync(ImplementationType* impl)
        {
            winrt::handle completionEvent{ CreateEvent(nullptr, TRUE, FALSE, nullptr) };
            
            impl->Closed([event = completionEvent.get()](auto&&, auto&&) { SetEvent(event); });
            impl->Activate();
            
            co_await winrt::resume_on_signal(completionEvent.get());
            co_return impl->Result;
        }
    }

    App::App()
    {
        InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::UnhandledExceptionEventArgs const& e)
            {
                if (IsDebuggerPresent())
                {
                    auto errorMessage = e.Message();
                    __debugbreak();
                }
            });
#endif
    }

    void App::OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const&)
    {
        CommandLineHandler::AppLaunchMode == AppLaunchMode::LaunchSettings ? launchSettings() : normalLaunch();
    }

    void App::launchSettings()
    {
        m_settingsLock.emplace();
        Settings settings;
        if (settings.Get(Settings::IsFirstLaunch, true))
        {
            winrt::FastCopy::WelcomeWindow window;
            window.Activate();
            settings.Set(Settings::IsFirstLaunch, false);
        }
        else
        {
            winrt::FastCopy::SettingsWindow window;
            Global::windowEffectHelper.SetTarget(window);
            window.Activate();
        }
    }

    bool winrt::FastCopy::implementation::App::HasAnotherInstance()
    {
        return !FastcopySettingsSingleInstanceLock{ false }.TryLock();
    }

    void App::normalLaunch()
    {
        auto const destination = Command::Get().GetDestination();
        //destination could be empty if delete is invoked
        auto const recordFile = Command::Get().RecordFile();
        auto viewModelRef = ViewModelLocator::GetInstance().RobocopyViewModel();
        auto viewModel = winrt::get_self<winrt::FastCopy::implementation::RobocopyViewModel>(viewModelRef);
#ifndef _DEBUG
        if (recordFile.empty())
            return;
#endif // !_DEBUG


        viewModel->Destination(destination);
        viewModel->RecordFile(winrt::hstring{ recordFile });

        if (viewModel->m_recordFile->GetOperation() == CopyOperation::Delete && Settings{}.Get(Settings::ConfirmDelete, true))
        {
            showConfirmDeleteWindowIfNeeded(viewModel);
            return;
        }

        LOGI(L"App started, record file: {}", recordFile.data());

        winrt::FastCopy::CopyDialogWindow window;
        Global::windowEffectHelper.SetTarget(window);
        Global::windowEffectHelper.SetListenThemeChange();
        window.Activate();
        viewModel->Start();
    }

    winrt::fire_and_forget App::showConfirmDeleteWindowIfNeeded(RobocopyViewModel* viewModel)
    {
        auto& taskFile = *viewModel->m_recordFile;
        auto const itemCount = std::distance(taskFile.begin(), taskFile.end());
        
        if (itemCount == 0)
            co_return;

        if (itemCount > 1)
        {
            if (!co_await ShowConfirmWindowAsync(winrt::make_self<ConfirmDeleteMultipleItemsWindow>().get()))
                co_return;
        }
        else if (std::filesystem::is_directory(*taskFile.begin()))
        {
            if (!co_await ShowConfirmWindowAsync(winrt::make_self<ConfirmDeleteFolderWindow>().get()))
                co_return;
        }
        else
        {
            if (!co_await ShowConfirmWindowAsync(winrt::make_self<ConfirmDeleteFileWindow>().get()))
                co_return;
        }

        LOGI(L"App started (after delete confirmation), record file: {}", taskFile.GetPath().data());
        
        winrt::FastCopy::CopyDialogWindow window;
        Global::windowEffectHelper.SetTarget(window);
        Global::windowEffectHelper.SetListenThemeChange();
        window.Activate();
        viewModel->Start();
    }
}
