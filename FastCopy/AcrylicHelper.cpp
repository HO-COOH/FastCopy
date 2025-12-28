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
{
    m_target = window;
    init();
}

void WindowEffectHelper::UpdateBackgroundForActualTheme()
{
    if (cachedEffect != 2)
        return;

    if (auto page = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>())
    {
        auto actualTheme = page.ActualTheme();
        page.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{ actualTheme == winrt::Microsoft::UI::Xaml::ElementTheme::Light ? winrt::Windows::UI::Colors::White() : winrt::Windows::UI::Colors::Black() });
    }
    else if (auto grid = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>())
    {
        auto actualTheme = grid.ActualTheme();
        grid.Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{ actualTheme == winrt::Microsoft::UI::Xaml::ElementTheme::Light ? winrt::Windows::UI::Colors::White() : winrt::Windows::UI::Colors::Black() });
    }
}

void WindowEffectHelper::SetEffect(int value)
{
    cachedEffect = value;
    
    // Revoke previous ActualThemeChanged listener
    m_themeChangedRevoker.revoke();

    switch (value)
    {
        case 0:
            m_target.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::MicaBackdrop{});
            // Clear solid background when using backdrop effect
            if (auto page = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>())
                page.Background(nullptr);
            else if (auto grid = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>())
                grid.Background(nullptr);
            break;
        case 1:
            m_target.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::DesktopAcrylicBackdrop{});
            // Clear solid background when using backdrop effect
            if (auto page = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>())
                page.Background(nullptr);
            else if (auto grid = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>())
                grid.Background(nullptr);
            break;
        case 2:
        {
            m_target.SystemBackdrop(nullptr);
            // Set solid background based on actual theme when backdrop is disabled
            UpdateBackgroundForActualTheme();
            
            // Listen for Windows system theme changes when using disabled effect
            if (auto page = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>())
            {
                m_themeChangedRevoker = page.ActualThemeChanged(winrt::auto_revoke, [this](auto&&, auto&&) {
                    UpdateBackgroundForActualTheme();
                });
            }
            else if (auto grid = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>())
            {
                m_themeChangedRevoker = grid.ActualThemeChanged(winrt::auto_revoke, [this](auto&&, auto&&) {
                    UpdateBackgroundForActualTheme();
                });
            }
            break;
        }
        default:
            break;
    }
}

void WindowEffectHelper::SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if (auto page = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>())
    {
        page.RequestedTheme(theme);
        if (cachedEffect == 2)
            UpdateBackgroundForActualTheme();
        else
            page.Background(nullptr);
    }
    else if (auto grid = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Grid>())
    {
        grid.RequestedTheme(theme);
        if (cachedEffect == 2)
            UpdateBackgroundForActualTheme();
        else
            grid.Background(nullptr);
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
