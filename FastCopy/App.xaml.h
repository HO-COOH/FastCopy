// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "App.xaml.g.h"
#include <optional>
#include <winrt/Microsoft.Windows.AppLifecycle.h>
#include "AppLaunchMode.h"
#include "FastcopySettingsSingleInstanceLock.h"
#include "RobocopyViewModel.h"

namespace winrt::FastCopy::implementation
{
    struct App : AppT<App>
    {
        App();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);


        /**
         * @brief Checks whether there is another instance of FastCopy running in settings window
         */
        static bool HasAnotherInstance();
    private:
        std::optional<FastcopySettingsSingleInstanceLock> m_settingsLock;

        /**
         * @brief Open the settings GUI.
         */
        void launchSettings();

        /**
         * @brief This should be the last function to call in `App::OnLaunched()`, it runs all the copy logic.
         */
        void normalLaunch();

        /*
        * @brief Shows the appropriate confirm delete window based on item count and type.
        * @param viewModel Pointer to the RobocopyViewModel implementation
        */
        winrt::fire_and_forget showConfirmDeleteWindowIfNeeded(RobocopyViewModel* viewModel);
    };
}
