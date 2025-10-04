#pragma once

#include "SettingsWindow.g.h"

namespace winrt::FastCopy::implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow>
    {
        SettingsWindow();

        winrt::FastCopy::SettingsViewModel ViewModel();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingsWindow : SettingsWindowT<SettingsWindow, implementation::SettingsWindow>
    {
    };
}
