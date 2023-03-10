#include "pch.h"
#include "AcrylicHelper.h"
#include <winrt/Windows.System.h>
#include <windows.system.h>
#include <DispatcherQueue.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>

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

AcrylicHelper::AcrylicHelper(winrt::Microsoft::UI::Xaml::Window& window):m_window{window}
{
    EnsureWindowsSystemDispatcherQueueController();
    trySetAcrylicBackdrop();
}

bool AcrylicHelper::trySetAcrylicBackdrop()
{
    auto activatedToken = m_window.Activated([this](auto, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs args)
        {
            if (m_configurationSource)
                m_configurationSource.IsInputActive(args.WindowActivationState() != winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated);
        });

    m_window.Closed(
        [this, activatedToken](auto, winrt::Microsoft::UI::Xaml::WindowEventArgs args)
        {
            m_acrylicController = nullptr;
            m_configurationSource = nullptr;
            m_window.Activated(activatedToken);
        }
    );
    m_configurationSource.IsInputActive(true);
    setConfigurationSourceTheme();
    auto pt = m_window.try_as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>();
    m_acrylicController.SetSystemBackdropConfiguration(m_configurationSource);
    m_acrylicController.AddSystemBackdropTarget(pt);

    return true;
}

void AcrylicHelper::setConfigurationSourceTheme()
{
    switch (m_window.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().ActualTheme())
    {
        case winrt::Microsoft::UI::Xaml::ElementTheme::Dark:    m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Dark); break;
        case winrt::Microsoft::UI::Xaml::ElementTheme::Light:   m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Light); break;
        case winrt::Microsoft::UI::Xaml::ElementTheme::Default:   m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Default); break;
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

MicaHelper::MicaHelper(winrt::Microsoft::UI::Xaml::Window& window) : m_window{window}
{
    EnsureWindowsSystemDispatcherQueueController();
    trySetMicaBackdrop();
}

bool MicaHelper::trySetMicaBackdrop()
{
    auto activatedToken = m_window.Activated([this](auto, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs args)
        {
            if (m_configurationSource)
            m_configurationSource.IsInputActive(args.WindowActivationState() != winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated);
        });

    m_window.Closed(
        [this, activatedToken](auto, winrt::Microsoft::UI::Xaml::WindowEventArgs args)
        {
            m_micaController = nullptr;
    m_configurationSource = nullptr;
    m_window.Activated(activatedToken);
        }
    );
    m_configurationSource.IsInputActive(true);
    setConfigurationSourceTheme();
    auto pt = m_window.try_as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>();
    m_micaController.SetSystemBackdropConfiguration(m_configurationSource);
    m_micaController.AddSystemBackdropTarget(pt);
    m_micaController.Kind(winrt::Microsoft::UI::Composition::SystemBackdrops::MicaKind::BaseAlt);

    return true;
}

void MicaHelper::setConfigurationSourceTheme()
{
    switch (m_window.Content().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().ActualTheme())
    {
    case winrt::Microsoft::UI::Xaml::ElementTheme::Dark:    m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Dark); break;
    case winrt::Microsoft::UI::Xaml::ElementTheme::Light:   m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Light); break;
    case winrt::Microsoft::UI::Xaml::ElementTheme::Default:   m_configurationSource.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Default); break;
    }
}
