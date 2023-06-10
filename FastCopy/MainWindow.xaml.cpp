// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "WindowHelper.h"
#include "UIDispatcher.h"


using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{

    MainWindow::MainWindow()
    {
        InitializeComponent();
        
        AppWindow().Title(L"FastCopy");
        ResizeWindow(*this, Size);
        UIDispatcher::g_dispatcher = DispatcherQueue();
        UIDispatcher::g_mainWindow = *this;
    }

}
