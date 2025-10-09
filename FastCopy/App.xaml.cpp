// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"

#include "App.xaml.h"
#include "CopyDialogWindow.xaml.h"
#include "SettingsWindow.xaml.h"
#include "ViewModelLocator.h"
#include "FastCopyCommandParser.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "CommandLine.h"
#include "DebugLogger.h"
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include <winrt/Windows.System.h>
#include <filesystem>
#include "Global.h"
#include "RobocopyViewModel.h"
#include "CommandLineHandler.h"


namespace winrt::FastCopy::implementation
{
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
        return CommandLineHandler::AppLaunchMode == AppLaunchMode::LaunchSettings ? launchSettings() : normalLaunch();
    }

    void App::launchSettings()
    {
        m_settingsLock.emplace();
        setting = make<SettingsWindow>();
        setting.Activate();
        Global::windowEffectHelper.SetTarget(setting);
    }

    bool winrt::FastCopy::implementation::App::HasAnotherInstance()
    {
        return !FastcopySettingsSingleInstanceLock{ false }.TryLock();
    }

    void App::normalLaunch()
    {
        auto const destination = Command::Get().GetDestination();
        if (destination.empty())
            return;

        auto const recordFile = Command::Get().RecordFile();
        auto viewModelRef = ViewModelLocator::GetInstance().RobocopyViewModel();
        auto viewModel = winrt::get_self<winrt::FastCopy::implementation::RobocopyViewModel>(viewModelRef);
#ifndef _DEBUG
        if (recordFile.empty())
            return;
#endif // !_DEBUG


        viewModel->Destination(destination);
        viewModel->RecordFile(winrt::hstring{ recordFile });
        viewModel->Finished([recordFile](auto, FinishState state) {
#ifndef _DEBUG
            if (state == FinishState::Success)
                std::filesystem::remove(recordFile.data());
#endif
            });

        LOGI(L"App started, record file: {}", recordFile.data());

        copyDialogWindow = make<CopyDialogWindow>();
        copyDialogWindow.Activate();
        Global::windowEffectHelper.SetTarget(copyDialogWindow);
        Global::windowEffectHelper.SetListenThemeChange();
        viewModel->Start();
    }

}