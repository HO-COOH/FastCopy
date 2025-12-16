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

        int RenameBehavior();
        void RenameBehavior(int value);

        winrt::hstring RenameSuffix();
        void RenameSuffix(winrt::hstring const& value);

        int MultipleWindowBehavior();
        void MultipleWindowBehavior(int value);

        int ThemeSelection();
        void ThemeSelection(int value);

        int BackgroundSelection();
        void BackgroundSelection(int value);

        bool DevMode();
        void DevMode(bool value);

        static winrt::Microsoft::UI::Xaml::Visibility IsRenameTextBoxVisible(int renameBehavior);
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
