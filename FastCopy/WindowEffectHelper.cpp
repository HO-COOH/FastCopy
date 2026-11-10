#include "pch.h"
#include "WindowEffectHelper.h"
#include <winrt/Windows.System.h>
#include <windows.system.h>
#include <DispatcherQueue.h>
#include "SettingsChangeListener.h"
#include "ViewModelLocator.h"
#include "SettingsViewModel.h"

void WindowEffectHelper::init()
{
    auto settingsViewModel = ViewModelLocator::GetInstance().SettingsViewModel();
    SetEffect(settingsViewModel->BackgroundSelection());
    SetTheme(static_cast<winrt::Microsoft::UI::Xaml::ElementTheme>(settingsViewModel->ThemeSelection()));
    SettingsChangeListener::GetInstance().BroadcastThemeChange();
}

void WindowEffectHelper::SetTarget(
    winrt::WinUI3Package::WindowEx window,
    winrt::Microsoft::UI::Xaml::Controls::Panel backgroundElement)
{
    m_themeChangedRevoker.revoke();
    m_window = std::move(window);
    m_backgroundElement = std::move(backgroundElement);
    // This helper outlives the window, so don't keep the window alive past its close
    m_window.Closed([this](auto&&, auto&&)
    {
        m_themeChangedRevoker.revoke();
        m_window = nullptr;
        m_backgroundElement = nullptr;
    });
    init();
}

void WindowEffectHelper::UpdateBackgroundForActualTheme()
{
    if (cachedEffect != 2)
        return;

    auto actualTheme = m_backgroundElement.ActualTheme();
    m_backgroundElement.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{ actualTheme == winrt::Microsoft::UI::Xaml::ElementTheme::Light ? winrt::Windows::UI::Colors::White() : winrt::Windows::UI::Colors::Black() });
}

void WindowEffectHelper::SetEffect(int value)
{
    cachedEffect = value;

    // Revoke previous ActualThemeChanged listener
    m_themeChangedRevoker.revoke();

    if (!m_window)
        return;

    switch (value)
    {
        case 0:
            if (winrt::Microsoft::UI::Composition::SystemBackdrops::MicaController::IsSupported())
                m_window.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::MicaBackdrop{});
            else
            {
                // Simulated Mica for Windows 10. Unlike the built-in backdrops it doesn't pick up the theme from Window.Content,
                // so bind it to the element SetTheme() sets RequestedTheme on
                winrt::WinUI3Package::TenMicaBackdrop backdrop;
                backdrop.BindThemeTo(m_window);
                m_window.SystemBackdrop(backdrop);
            }
            // Clear solid background when using backdrop effect
            m_backgroundElement.Background(nullptr);
            break;
        case 1:
            m_window.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::DesktopAcrylicBackdrop{});
            // Clear solid background when using backdrop effect
            m_backgroundElement.Background(nullptr);
            break;
        case 2:
        {
            m_window.SystemBackdrop(nullptr);
            // Set solid background based on actual theme when backdrop is disabled
            UpdateBackgroundForActualTheme();

            // Listen for Windows system theme changes when using disabled effect
            m_themeChangedRevoker = m_backgroundElement.ActualThemeChanged(winrt::auto_revoke, [this](auto&&, auto&&) {
                UpdateBackgroundForActualTheme();
            });
            break;
        }
        default:
            break;
    }
}

void WindowEffectHelper::SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if (!m_window)
        return;

    m_window.RequestedTheme(theme);
    if (cachedEffect == 2)
        UpdateBackgroundForActualTheme();
    else
        m_backgroundElement.Background(nullptr);
}

void WindowEffectHelper::SetListenThemeChange()
{
    SettingsChangeListener::GetInstance().OnThemeChange([this](SettingsChangeListener::ThemeChangeEventArg e)
    {
        SetEffect(e.effect);
        SetTheme(static_cast<winrt::Microsoft::UI::Xaml::ElementTheme>(e.theme));
    });
}
