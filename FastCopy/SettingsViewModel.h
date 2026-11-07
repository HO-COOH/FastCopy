#pragma once

#include "SettingsViewModel.g.h"
#include "Settings.h"
#include <include/PropertyChangeHelper.hpp>

class WindowEffectHelper;

namespace winrt::FastCopy::implementation
{
    struct SettingsViewModel : SettingsViewModelT<SettingsViewModel>, MvvmHelper::PropertyChangeHelper<SettingsViewModel>
    {
        bool Notify();
        void Notify(bool value);

        bool KeyboardIntegration();
        void KeyboardIntegration(bool value);

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

        bool ConfirmDelete();
        void ConfirmDelete(bool value);

        static winrt::Microsoft::UI::Xaml::Visibility IsRenameTextBoxVisible(int renameBehavior);
        static double RenameSuffixErrorOpacity(winrt::hstring const& suffix);
        static winrt::Microsoft::UI::Xaml::Visibility RenameSuffixErrorVisibility(winrt::hstring const& suffix);

        WindowEffectHelper* WindowEffectHelperPtr{};
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
