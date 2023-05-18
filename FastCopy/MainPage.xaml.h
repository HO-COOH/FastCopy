// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainPage.g.h"

namespace winrt::FastCopy::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        winrt::FastCopy::SettingsViewModel ViewModel();
        

    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
