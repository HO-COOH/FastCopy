#include "pch.h"
#include "AcrylicHelper.h"
#include <winrt/Windows.System.h>
#include <windows.system.h>
#include <DispatcherQueue.h>
#include "SettingsChangeListener.h"
#include "ViewModelLocator.h"


void WindowEffectHelper::init()
{
    if (!m_target)
        return;

    auto settingsViewModel = ViewModelLocator::GetInstance().SettingsViewModel();
    settingsViewModel.BackgroundSelection(settingsViewModel.BackgroundSelection());
    settingsViewModel.ThemeSelection(settingsViewModel.ThemeSelection());
}

void WindowEffectHelper::SetTarget(winrt::Microsoft::UI::Xaml::Window window)
{    m_target = window;
    init();
}

void WindowEffectHelper::SetEffect(int value)
{
    switch (value)
    {
        case 0:
            m_target.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::MicaBackdrop{});
            break;
        case 1:
            m_target.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::DesktopAcrylicBackdrop{});
            break;
        case 2:
            m_target.SystemBackdrop(nullptr);
            break;
        default:
            break;
    }
}

void WindowEffectHelper::SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if (auto element = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>(); element)
    {
        element.RequestedTheme(theme);
    }
}

void WindowEffectHelper::SetListenThemeChange()
{
    SettingsChangeListener::GetInstance().OnThemeChange([this](SettingsChangeListener::ThemeChangeEventArg e)
    {
        SetEffect(e.effect);
        SetTheme(static_cast<winrt::Microsoft::UI::Xaml::ElementTheme>(e.theme));
    });
}
