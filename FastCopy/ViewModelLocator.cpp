#include "pch.h"
#include "ViewModelLocator.h"
#include "RobocopyViewModel.h"
#include "SettingsViewModel.h"

ViewModelLocator& ViewModelLocator::GetInstance()
{
    static ViewModelLocator s_locator;
    return s_locator;
}

winrt::FastCopy::implementation::RobocopyViewModel* ViewModelLocator::RobocopyViewModel()
{
    if (!m_robocopyViewModel)
        m_robocopyViewModel = winrt::make_self<winrt::FastCopy::implementation::RobocopyViewModel>();
    return m_robocopyViewModel.get();
}

winrt::FastCopy::implementation::SettingsViewModel* ViewModelLocator::SettingsViewModel()
{
    if (!m_settingsViewModel)
        m_settingsViewModel = winrt::make_self<winrt::FastCopy::implementation::SettingsViewModel>();
    return m_settingsViewModel.get();
}

SpeedGraphData& ViewModelLocator::SpeedGraphData()
{
    return m_speedGraphData;
}
