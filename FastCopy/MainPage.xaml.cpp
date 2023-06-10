// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include "ViewModelLocator.h"
#include "SettingsChangeListener.h"

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
        SettingsChangeListener::GetInstance().OnThemeChange(
            [this](SettingsChangeListener::ThemeChangeEventArg e)
            {
                if (e.effect == 2)
                {
                    switch (e.theme)
                    {
                    case 0: Background(nullptr); break;
                    case 1: Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{winrt::Windows::UI::Colors::White()}); break;
                    case 2: Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{winrt::Windows::UI::Colors::Black()}); break;
                    }
                }
                else
                    Background(nullptr);
            }
        );
    }

    winrt::FastCopy::SettingsViewModel MainPage::ViewModel()
    {
        return ViewModelLocator::GetInstance().SettingsViewModel();
    }


}

