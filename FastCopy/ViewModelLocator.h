#pragma once
#include "SpeedGraphData.h"

namespace winrt::FastCopy
{
	struct RobocopyViewModel;
	struct SettingsViewModel;

	namespace implementation
	{
		struct RobocopyViewModel;
		struct SettingsViewModel;
	}
}

struct ViewModelLocator
{
	winrt::com_ptr<winrt::FastCopy::implementation::RobocopyViewModel> m_robocopyViewModel;
	winrt::com_ptr<winrt::FastCopy::implementation::SettingsViewModel> m_settingsViewModel;
	SpeedGraphData m_speedGraphData;

	static ViewModelLocator& GetInstance();
	winrt::FastCopy::implementation::RobocopyViewModel* RobocopyViewModel();
	winrt::FastCopy::implementation::SettingsViewModel* SettingsViewModel();
	SpeedGraphData& SpeedGraphData();
};

