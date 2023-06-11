#pragma once
#include "RobocopyViewModel.g.h"
#include "SettingsViewModel.g.h"
class ViewModelLocator
{
	winrt::FastCopy::RobocopyViewModel m_robocopyViewModel{ nullptr };
	winrt::FastCopy::SettingsViewModel m_settingsViewModel{ nullptr };
public:
	static ViewModelLocator& GetInstance();
	winrt::FastCopy::RobocopyViewModel RobocopyViewModel();
	winrt::FastCopy::SettingsViewModel SettingsViewModel();
};

