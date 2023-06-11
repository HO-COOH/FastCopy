// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "WindowEx.g.h"
#include "WindowEx.h"
#include "MainWindow.g.h"
#include "MutexWrapper.h"


namespace winrt::FastCopy::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
    private:
        //Unscaled window size
        constexpr static winrt::Windows::Graphics::SizeInt32 Size
        {
            600, 470
        };

        MutexWrapper m_settingsLock{ L"FastCopySettingsLock", true };
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
