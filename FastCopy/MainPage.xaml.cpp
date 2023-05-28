// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include "ViewModelLocator.h"
#include "Global.h"

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
    }

    winrt::FastCopy::SettingsViewModel MainPage::ViewModel()
    {
        return ViewModelLocator::GetInstance().SettingsViewModel();
    }


}

