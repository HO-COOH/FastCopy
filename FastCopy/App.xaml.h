// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "App.xaml.g.h"
#include "AcrylicHelper.h"
#include <optional>
#include <winrt/Microsoft.Windows.AppLifecycle.h>

namespace winrt::FastCopy::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);
    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        winrt::Microsoft::UI::Xaml::Window setting{ nullptr };

        static std::optional<winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments> isLaunchByToastNotification();
        static void launchByToastNotification(winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments const& args);

        static bool isLaunchSettings();
        void launchSettings();

        void normalLaunch();

        std::optional<MicaHelper> m_helper;
    };
}
