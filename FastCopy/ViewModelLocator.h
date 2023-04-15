#pragma once

#include "XCopyViewModel.g.h"
class ViewModelLocator
{
	winrt::FastCopy::XCopyViewModel m_viewModel{ nullptr };
	winrt::FastCopy::RobocopyViewModel m_robocopyViewModel{ nullptr };
public:
	static ViewModelLocator& GetInstance();
	winrt::FastCopy::XCopyViewModel XCopyViewModel();
	winrt::FastCopy::RobocopyViewModel RobocopyViewModel();
};

