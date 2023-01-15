// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once

#include "MainWindow.g.h"

//namespace winrt
//{
//    namespace MUC = Microsoft::UI::Composition;
//    namespace MUCSB = Microsoft::UI::Composition::SystemBackdrops;
//    namespace MUX = Microsoft::UI::Xaml;
//    namespace WS = Windows::System;
//}

namespace winrt::FastCopy::implementation
{


    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}



