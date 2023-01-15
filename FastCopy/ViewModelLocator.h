#pragma once

#include "XCopyViewModel.g.h"
class ViewModelLocator
{
	winrt::FastCopy::XCopyViewModel m_viewModel{ nullptr };
public:
	static ViewModelLocator& GetInstance();
	winrt::FastCopy::XCopyViewModel XCopyViewModel();
};

