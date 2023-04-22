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


	void CopyDialog::ProgressBar_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		if (ProgressBar().ActualWidth() != 0 && !std::isnan(ProgressBar().ActualWidth()))
		{
			WidthAnimation().To(ProgressBar().ActualWidth());
			ProgressBarEntranceAnimation().Begin();
		}

		ViewModelLocator::GetInstance().RobocopyViewModel().Finished(
			[](auto, FinishState state)
			{
				OutputDebugString(L"Called finished\n");
			}
		);
	}

}


void winrt::FastCopy::implementation::CopyDialog::ProgressBar_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
{

}
