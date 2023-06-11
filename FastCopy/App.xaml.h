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

        /**
         * @brief Check if launched by toast notification, if is, return the argument
         * @details Currently there is [Open] and [Dismiss]
         */
        static std::optional<winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments> isLaunchByToastNotification();

        /**
         * @brief Handle launch by toast notification
         * @details Currently only [Open] is handled. It opens the path in `explorer.exe` then exit
         */
        static void launchByToastNotification(winrt::Microsoft::Windows::AppLifecycle::AppActivationArguments const& args);

        /**
         * @brief Check if launched by running the main program
         */
        static bool isLaunchSettings();

        /**
         * @brief Open the settings GUI.
         */
        void launchSettings();

        /**
         * @brief Checks whether there is another instance of FastCopy running in settings window
         */
        static bool hasAnotherInstance();

        /**
         * @brief This should be the last function to call in `App::OnLaunched()`, it runs all the copy logic.
         */
        void normalLaunch();
    };
}
