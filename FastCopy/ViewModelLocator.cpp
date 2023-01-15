#include "pch.h"
#include "ViewModelLocator.h"

ViewModelLocator& ViewModelLocator::GetInstance()
{
    static ViewModelLocator s_locator;
    return s_locator;
}

winrt::FastCopy::XCopyViewModel ViewModelLocator::XCopyViewModel()
{
    if (!m_viewModel)
        m_viewModel = winrt::FastCopy::XCopyViewModel();

    return m_viewModel;
}
