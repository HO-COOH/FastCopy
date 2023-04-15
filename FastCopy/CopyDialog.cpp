#include "pch.h"
#include "CopyDialog.h"
#if __has_include("CopyDialog.g.cpp")
#include "CopyDialog.g.cpp"
#endif

#include "ViewModelLocator.h"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::FastCopy::implementation
{
	winrt::FastCopy::RobocopyViewModel CopyDialog::ViewModel()
	{
		return ViewModelLocator::GetInstance().RobocopyViewModel();
	}
}
