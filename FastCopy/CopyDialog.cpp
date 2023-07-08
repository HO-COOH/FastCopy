#include "pch.h"
#include "CopyDialog.h"
#if __has_include("CopyDialog.g.cpp")
#include "CopyDialog.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <winrt/Windows.System.h>
#include "Taskbar.h"
#include "Global.h"
#include "SettingsChangeListener.h"

#include "FileContextMenu.h"
#pragma comment(lib, "gdiplus.lib")

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
	}

	void CopyDialog::HyperlinkButton_Click(
		winrt::Windows::Foundation::IInspectable const& sender, 
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		winrt::Windows::System::Launcher::LaunchUriAsync(ViewModel().DestinationUri());
	}


	void CopyDialog::PauseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		if (PauseIcon().Symbol() == winrt::Microsoft::UI::Xaml::Controls::Symbol::Pause)
		{
			ViewModel().Pause();
			PauseIcon().Symbol(winrt::Microsoft::UI::Xaml::Controls::Symbol::Play);
			ProgressBar().Color(winrt::Windows::UI::Colors::Yellow());
			Taskbar::SetProgressState(Global::MainHwnd, Taskbar::ProgressState::Paused);
		}
		else
		{
			ViewModel().Start();
			PauseIcon().Symbol(winrt::Microsoft::UI::Xaml::Controls::Symbol::Pause);
			ProgressBar().Color(winrt::Windows::UI::Colors::Green());
			Taskbar::SetProgressState(Global::MainHwnd, Taskbar::ProgressState::Normal);
		}
	}

	CopyDialog::CopyDialog()
	{
		InitializeComponent();
		ViewModel().DuplicateFiles().VectorChanged(
			[this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto)
			{
				if (original.Size() == 0)
				{
					UnloadObject(DuplicateFileInfoGrid());
					UnloadObject(ContinueButton());
				}
				else
				{
					DispatcherQueue().TryEnqueue([this] 
					{
						FindName(L"ContinueButton");
						FindName(L"DuplicateFileItems");
						FindName(L"DuplicateFileInfoGrid");
					});
				}
			}
		);
		SettingsChangeListener::GetInstance().OnThemeChange(
			[this](SettingsChangeListener::ThemeChangeEventArg e)
			{
				if (e.effect == 2)
				{
					switch (e.theme)
					{
					case 0: Background(nullptr); break;
					case 1: Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{winrt::Windows::UI::Colors::White()}); break;
					case 2: Background(winrt::Microsoft::UI::Xaml::Media::SolidColorBrush{winrt::Windows::UI::Colors::Black()}); break;
					}
				}
				else
					Background(nullptr);
			}
		);
		ViewModelLocator::GetInstance().RobocopyViewModel().DuplicateFiles().VectorChanged(
			[this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto)
			{
				winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(
					*this,
					original.Size() == 0 ? L"HideDuplicateFiles" : L"ShowDuplicateFiles",
					false
				);
			}
		);
	}

	void CopyDialog::MainPanel_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
	{
		//The width will not automatically resized after applying storyboard animation
		//need this function to set it manually
		auto width = e.NewSize().Width;
		if (width != 0 && !std::isnan(width))
			ProgressBar().Width(width);
	}

	void CopyDialog::ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		if (m_showSpeedGraph)
		{
			HideGraphAnimation().Completed([this](auto...) { UnloadObject(SpeedGraph()); });
			HideGraphAnimation().Begin();
			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"HideGraph", false);
			Global::copyWindow.Resize(m_originalWindowSize);
			ViewModel().PropertyChanged(m_speedUpdateRevoker);
		}
		else
		{
			FindName(L"SpeedGraph").as<winrt::FastCopy::SpeedGraph>().Loaded([this](auto...)
			{ 
				GraphAnimation().Begin(); 
			});
			m_speedUpdateRevoker = ViewModel().PropertyChanged([this](auto, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs e) {
				if (e.PropertyName() == L"Speed")
				{
					SpeedGraph().SetSpeed(ViewModel().Percent(), ViewModel().Speed());
				}
			});

			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"ShowGraph", false);
			m_originalWindowSize = Global::copyWindow.ActualSize();
			Global::copyWindow.Resize({ (int)m_originalWindowSize.Width, (int)m_originalWindowSize.Height + 190 });
		}
		m_showSpeedGraph = !m_showSpeedGraph;
	}

	void CopyDialog::ShowMenu(winrt::hstring path, winrt::Microsoft::UI::Xaml::Controls::MenuFlyout flyout)
	{
		std::make_shared<FileContextMenu>(path)->ShowAt(flyout);
	}

}


void winrt::FastCopy::implementation::CopyDialog::CheckBox_Checked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	OutputDebugString(L"Checkbox clicked\n");
}


void winrt::FastCopy::implementation::CopyDialog::MenuFlyout_Opening(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& e)
{
	auto menu = sender
		.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyout>();
	auto fileInfoViewModel = menu
		.GetValue(winrt::FastCopy::DataInjector::DataProperty())
		.as<winrt::FastCopy::FileInfoViewModel>();

	ShowMenu(fileInfoViewModel.Path(), menu);
}
