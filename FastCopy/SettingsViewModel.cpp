#include "pch.h"
#include "SettingsViewModel.h"
#if __has_include("SettingsViewModel.g.cpp")
#include "SettingsViewModel.g.cpp"
#endif
#include "Global.h"
#include "SettingsChangeListener.h"

namespace FCCS = ::FastCopy::Settings;

namespace winrt::FastCopy::implementation
{
    SettingsViewModel::SettingsViewModel()
    {
        m_dispatcher = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
        
        FCCS::CommonSharedSettings::Instance().RegisterChangeListener(
            &SettingsViewModel::OnSharedSettingsChanged,
            this);
        m_sharedSettingsSubscribed = true;
    }

    SettingsViewModel::~SettingsViewModel()
    {
        if (m_sharedSettingsSubscribed)
        {
            FCCS::CommonSharedSettings::Instance().UnregisterChangeListener(
                &SettingsViewModel::OnSharedSettingsChanged,
                this);
        }
    }

    bool SettingsViewModel::Notify()
    {
        return m_model.Get<bool>(Settings::Notify, true);
    }
    void SettingsViewModel::Notify(bool value)
    {
        m_model.Set(Settings::Notify, value);
    }
    int SettingsViewModel::MultipleWindowBehavior()
    {
        return m_model.Get<int>(Settings::MultipleWindowBehavior, 0);
    }
    void SettingsViewModel::MultipleWindowBehavior(int value)
    {
        m_model.Set(Settings::MultipleWindowBehavior, value);
    }
    int SettingsViewModel::ThemeSelection()
    {
        return m_model.Get<int>(Settings::ThemeSelection, 0);
    }
    void SettingsViewModel::ThemeSelection(int value)
    {
        m_model.Set(Settings::ThemeSelection, value);
        Global::windowEffectHelper.SetTheme(static_cast<winrt::Microsoft::UI::Xaml::ElementTheme>(value));
        SettingsChangeListener::GetInstance().BroadcastThemeChange();
    }
    int SettingsViewModel::BackgroundSelection()
    {
        return m_model.Get<int>(Settings::BackgroundSelection, 0);
    }
    void SettingsViewModel::BackgroundSelection(int value)
    {
        m_model.Set(Settings::BackgroundSelection, value);
        Global::windowEffectHelper.SetEffect(value);
        SettingsChangeListener::GetInstance().BroadcastThemeChange();
    }
    bool SettingsViewModel::DevMode()
    {
        return m_model.Get(Settings::DevMode, false);
    }
    void SettingsViewModel::DevMode(bool value)
    {
        m_model.Set(Settings::DevMode, value);
    }

    bool SettingsViewModel::LoggerEnabled()
    {
        auto& s = FCCS::CommonSharedSettings::Instance();
        return s.GetBool(L"Logger", L"Enabled").value_or(false);
    }

    void SettingsViewModel::LoggerEnabled(bool value)
    {
        auto& s = FCCS::CommonSharedSettings::Instance();
        s.SetBool(L"Logger", L"Enabled", value);
    }

    int SettingsViewModel::LoggerVerbosity()
    {
        auto& s = FCCS::CommonSharedSettings::Instance();
        // Default 2 = Warn
        auto v = s.GetInt(L"Logger", L"Verbosity").value_or(2);
        if (v < 0) v = 0;
        if (v > 5) v = 5;
        return v;
    }

    void SettingsViewModel::LoggerVerbosity(int value)
    {
        if (value < 0) value = 0;
        if (value > 5) value = 5;

        auto& s = FCCS::CommonSharedSettings::Instance();
        s.SetInt(L"Logger", L"Verbosity", value);
    }

    bool SettingsViewModel::LogDBEnabled()
    {
        auto& s = FCCS::CommonSharedSettings::Instance();
        return s.GetBool(L"Logger", L"DebugBreakEnabled").value_or(false);
    }

    void SettingsViewModel::LogDBEnabled(bool value)
    {
        auto& s = FCCS::CommonSharedSettings::Instance();
        s.SetBool(L"Logger", L"DebugBreakEnabled", value);
    }

    int SettingsViewModel::LogDBMinVerbositySelectedIndex()
    {
        auto& s = FCCS::CommonSharedSettings::Instance();
        // default value = Debug(4)
        // 0 = None, 1 = Error, 2 = Warn, 3 = Info, 4 = Debug，5 = Trace
        auto v = s.GetInt(L"Logger", L"DebugBreakMinVerbosity").value_or(4);
        v = std::clamp(v - 1, 0, 4);
        return v;
    }

    void SettingsViewModel::LogDBMinVerbositySelectedIndex(int value)
    {
        value = std::clamp(value, 0, 4);
        value += 1; // 1..5

        auto& s = FCCS::CommonSharedSettings::Instance();
        s.SetInt(L"Logger", L"DebugBreakMinVerbosity", value);
    }

    void __stdcall SettingsViewModel::OnSharedSettingsChanged(void* ctx) noexcept
    {
        auto self = static_cast<SettingsViewModel*>(ctx);
        if (!self)
            return;

        auto &dispatcher = self->m_dispatcher;
        if (!dispatcher)
            return;

        //dispatcher.TryEnqueue([self]()
        //    {
        //        self->OnSharedSettingsChangedNotifyUI();
        //    });
        
        // Get a weak reference to the 'projection object'
        auto weak = self->get_weak();

        dispatcher.TryEnqueue([weak]()
            {
                if (auto strong = weak.get())
                {
                    // Back to implementation class
                    strong.get()->OnSharedSettingsChangedNotifyUI();
                }
            });
    }

    void SettingsViewModel::OnSharedSettingsChangedNotifyUI()
    {
        RaisePropertyChangedEvent({
            L"LoggerEnabled",
            L"LoggerVerbosity",
            L"LogDBEnabled",
            L"LogDBMinVerbositySelectedIndex"
            });
    }
}
