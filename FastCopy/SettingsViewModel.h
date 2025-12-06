#pragma once

#include "SettingsViewModel.g.h"
#include "Settings.h"
#include "CommonSharedSettings.h"
#include <winrt/Microsoft.UI.Dispatching.h>

namespace winrt::FastCopy::implementation
{
    struct SettingsViewModel : SettingsViewModelT<SettingsViewModel>
    {
        SettingsViewModel();
        ~SettingsViewModel();

        bool Notify();
        void Notify(bool value);

        int MultipleWindowBehavior();
        void MultipleWindowBehavior(int value);

        int ThemeSelection();
        void ThemeSelection(int value);

        int BackgroundSelection();
        void BackgroundSelection(int value);

        bool DevMode();
        void DevMode(bool value);

        bool LoggerEnabled();
        void LoggerEnabled(bool value);

        int LoggerVerbosity();              // 0~5 : Off..Trace
        void LoggerVerbosity(int value);

        bool LogDBEnabled();     // Is enabled to break when Logger Verbosity >= DebugBreakMinVerbosity
        void LogDBEnabled(bool value);

        int LogDBMinVerbositySelectedIndex(); // 0~4, for combobox index
        void LogDBMinVerbositySelectedIndex(int value);

#pragma region NotifyPropertyChanged Common Functions
        winrt::event_token PropertyChanged(winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler)
        {
            m_propertyChangedToken = m_eventPropertyChanged.add(handler);
            return m_propertyChangedToken;
        }
        void PropertyChanged(winrt::event_token token)
        {
            m_eventPropertyChanged.remove(token);
        }
        void RaisePropertyChangedEvent(std::wstring_view const& propertyName)
        {
            // Only instantiate the arguments class if the event has any listeners
            if (m_eventPropertyChanged)
            {
                winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs args{ propertyName };
                m_eventPropertyChanged(*this, args);
            }
        }

        void RaisePropertyChangedEvent(std::initializer_list<const std::wstring_view> const& propertyNames)
        {
            // Only instantiate the argumens class (and only once) if the event has any listeners
            if (m_eventPropertyChanged)
            {
                for (auto&& propertyName : propertyNames)
                {
                    winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs args{ propertyName };
                    m_eventPropertyChanged(*this, args);
                }
            }
        }
#pragma endregion

    private:
        static void __stdcall OnSharedSettingsChanged(void* ctx) noexcept;

        void OnSharedSettingsChangedNotifyUI();

        winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcher{ nullptr };
        bool m_sharedSettingsSubscribed{ false };
        winrt::event<winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_eventPropertyChanged;
        winrt::event_token m_propertyChangedToken{};

        Settings m_model;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct SettingsViewModel : SettingsViewModelT<SettingsViewModel, implementation::SettingsViewModel>
    {
    };
}
