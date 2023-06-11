#include "pch.h"
#include "ViewModelLocator.h"

ViewModelLocator& ViewModelLocator::GetInstance()
{
    static ViewModelLocator s_locator;
    return s_locator;
}

winrt::FastCopy::RobocopyViewModel ViewModelLocator::RobocopyViewModel()
{
    if (!m_robocopyViewModel)
        m_robocopyViewModel = winrt::FastCopy::RobocopyViewModel();
    return m_robocopyViewModel;
}

winrt::FastCopy::SettingsViewModel ViewModelLocator::SettingsViewModel()
{
    if (!m_settingsViewModel)
        m_settingsViewModel = winrt::FastCopy::SettingsViewModel();
    return m_settingsViewModel;
}
