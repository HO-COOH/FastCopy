#include "pch.h"
#include "AcrylicHelper.h"
#include <winrt/Windows.System.h>
#include <windows.system.h>
#include <DispatcherQueue.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include "SettingsChangeListener.h"
#include "ViewModelLocator.h"

void AcrylicHelper::EnsureWindowsSystemDispatcherQueueController()
{
    if (winrt::Windows::System::DispatcherQueue::GetForCurrentThread() != nullptr)
        return;


    DispatcherQueueOptions options
    {
    };
    options.dwSize = sizeof(options);
    options.apartmentType = DQTAT_COM_STA;
    options.threadType = DQTYPE_THREAD_CURRENT;

    ABI::Windows::System::IDispatcherQueueController* pt{};
    winrt::check_hresult(CreateDispatcherQueueController(options, &pt));
}

AcrylicHelper::AcrylicHelper(winrt::Microsoft::UI::Xaml::Window& window, AcrylicParameter parameter):m_window{window}
{
    EnsureWindowsSystemDispatcherQueueController();
    trySetAcrylicBackdrop(parameter);
}

AcrylicHelper::~AcrylicHelper()
{
    m_acrylicController.Close();
    m_acrylicController = nullptr;
}

bool AcrylicHelper::trySetAcrylicBackdrop(AcrylicParameter const& parameter)
{
    auto activatedToken = m_window.Activated([this](auto, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs args)
    {
        if (m_configurationSource)
            m_configurationSource.IsInputActive(args.WindowActivationState() != winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated);
    });
    //m_window.Closed(
    //    [this, activatedToken](auto, winrt::Microsoft::UI::Xaml::WindowEventArgs args)
    //    {
    //        //m_acrylicController = nullptr;
    //        //m_configurationSource = nullptr;
    //        //m_window.Activated(activatedToken);
    //    }
    //);

    m_window.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().ActualThemeChanged([this](auto, auto)
    {
        if(m_configurationSource)
            setConfigurationSourceTheme();
    });

    m_configurationSource.IsInputActive(true);
    setConfigurationSourceTheme();
    auto pt = m_window.try_as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>();

    if (parameter.fallbackColor) m_acrylicController.FallbackColor(*parameter.fallbackColor);
    if (parameter.luminosityOpacity) m_acrylicController.LuminosityOpacity(*parameter.luminosityOpacity);
    if (parameter.tintColor) m_acrylicController.TintColor(*parameter.tintColor);
    if (parameter.tintOpacity) m_acrylicController.TintOpacity(*parameter.tintOpacity);

    m_acrylicController.SetSystemBackdropConfiguration(m_configurationSource);
    m_acrylicController.AddSystemBackdropTarget(pt);

    return true;
}

void AcrylicHelper::setConfigurationSourceTheme()
{
    switch (m_window.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().ActualTheme())
    {
        case winrt::Microsoft::UI::Xaml::ElementTheme::Dark:    
            m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Dark); break;
        case winrt::Microsoft::UI::Xaml::ElementTheme::Light:   
            m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Light); break;
        case winrt::Microsoft::UI::Xaml::ElementTheme::Default:   
            m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Default); break;
    }
}

void MicaHelper::EnsureWindowsSystemDispatcherQueueController()
{
    if (winrt::Windows::System::DispatcherQueue::GetForCurrentThread() != nullptr)
        return;


    DispatcherQueueOptions options
    {
    };
    options.dwSize = sizeof(options);
    options.apartmentType = DQTAT_COM_STA;
    options.threadType = DQTYPE_THREAD_CURRENT;

    ABI::Windows::System::IDispatcherQueueController* pt{};
    winrt::check_hresult(CreateDispatcherQueueController(options, &pt));
}

MicaHelper::MicaHelper(winrt::Microsoft::UI::Xaml::Window& window, MicaParameter micaParameter) : m_window{window}
{
    EnsureWindowsSystemDispatcherQueueController();
    trySetMicaBackdrop(micaParameter);
}

MicaHelper::~MicaHelper()
{
    m_micaController.Close();
    m_micaController = nullptr;
}

bool MicaHelper::trySetMicaBackdrop(MicaParameter const& parameter)
{
    auto activatedToken = m_window.Activated([this](auto, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs args)
    {
        if (m_configurationSource)
            m_configurationSource.IsInputActive(args.WindowActivationState() != winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated);
    });

    //m_window.Closed([this, activatedToken](auto, winrt::Microsoft::UI::Xaml::WindowEventArgs args)
    //{
    //    //m_micaController = nullptr;
    //    //m_configurationSource = nullptr;
    //    //m_window.Activated(activatedToken);
    //});

    m_window.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().ActualThemeChanged([this](auto, auto)
    {
        if (m_configurationSource)
            setConfigurationSourceTheme();
    });

    m_configurationSource.IsInputActive(true);
    setConfigurationSourceTheme();
    auto pt = m_window.try_as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>();
    
    if (parameter.kind) m_micaController.Kind(*parameter.kind);
    if (parameter.fallbackColor) m_micaController.FallbackColor(*parameter.fallbackColor);
    if (parameter.luminosityOpacity) m_micaController.LuminosityOpacity(*parameter.luminosityOpacity);
    if (parameter.tintColor) m_micaController.TintColor(*parameter.tintColor);
    if (parameter.tintOpacity) m_micaController.TintOpacity(*parameter.tintOpacity);

    m_micaController.SetSystemBackdropConfiguration(m_configurationSource);
    m_micaController.AddSystemBackdropTarget(pt);

    return true;
}

void MicaHelper::setConfigurationSourceTheme()
{
    switch (m_window.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().ActualTheme())
    {
        case winrt::Microsoft::UI::Xaml::ElementTheme::Dark:    
            m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Dark); break;
        case winrt::Microsoft::UI::Xaml::ElementTheme::Light:   
            m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Light); break;
        case winrt::Microsoft::UI::Xaml::ElementTheme::Default:   
            m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Default); break;
    }
}

void WindowEffectHelper::init()
{
    if (!m_target)
        return;

    auto settingsViewModel = ViewModelLocator::GetInstance().SettingsViewModel();
    settingsViewModel.BackgroundSelection(settingsViewModel.BackgroundSelection());
    settingsViewModel.ThemeSelection(settingsViewModel.ThemeSelection());
}

void WindowEffectHelper::SetTheme(winrt::Microsoft::UI::Xaml::ElementTheme theme)
{
    if (auto element = m_target.Content().try_as<winrt::Microsoft::UI::Xaml::Controls::Page>(); element)
    {
        element.RequestedTheme(theme);

        if (m_helper.index() != 2)
        {
            element.Background(nullptr);
            return;
        }
    }
}

void WindowEffectHelper::SetListenThemeChange()
{
    SettingsChangeListener::GetInstance().OnThemeChange([this](SettingsChangeListener::ThemeChangeEventArg e)
    {
        if (e.effect != m_helper.index())
        {

            switch (e.effect)
            {
            case 0:
                TrySetMica();
                break;
            case 1:
                TrySetAcrylic();
                break;
            case 2:
                Reset();
                break;
            default:
                break;
            }
        }

        SetTheme(static_cast<winrt::Microsoft::UI::Xaml::ElementTheme>(e.theme));
    });
}
