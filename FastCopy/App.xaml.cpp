// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"

#include "App.xaml.h"
#include "CopyDialogWindow.xaml.h"
#include "SettingsWindow.xaml.h"
#include "WelcomeWindow.xaml.h"
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
        m_mainWindow.Activate();
    }

    void App::launchSettings()
    {
        m_settingsLock.emplace();
        if (Settings settings; settings.Get(Settings::IsFirstLaunch, true)/*true*/)
        {
            m_mainWindow = make<WelcomeWindow>();
            settings.Set(Settings::IsFirstLaunch, false);
        }
        else
        {
            m_mainWindow = make<SettingsWindow>();
            Global::windowEffectHelper.SetTarget(m_mainWindow);
        }
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

        LOGI(L"App started, record file: {}", recordFile.data());

        m_mainWindow = make<CopyDialogWindow>();
        Global::windowEffectHelper.SetTarget(m_mainWindow);
        Global::windowEffectHelper.SetListenThemeChange();
        viewModel->Start();
    }

}