#pragma once

#include "SettingsViewModel.g.h"
#include "Settings.h"

namespace winrt::FastCopy::implementation
{
    struct SettingsViewModel : SettingsViewModelT<SettingsViewModel>
    {
        SettingsViewModel() = default;

        bool Notify();
        void Notify(bool value);

        int MultipleWindowBehavior();
        void MultipleWindowBehavior(int value);

        int ThemeSelection();
        void ThemeSelection(int value);

        int BackgroundSelection();
        void BackgroundSelection(int value);
    private:
        Settings m_model;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingsViewModel : SettingsViewModelT<SettingsViewModel, implementation::SettingsViewModel>
    {
    };
}
