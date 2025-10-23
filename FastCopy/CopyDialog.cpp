#include "pch.h"
#include "CopyDialog.h"
#if __has_include("CopyDialog.g.cpp")
#include "CopyDialog.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <winrt/Windows.System.h>
#include "Global.h"
#include "SettingsChangeListener.h"
#include "FileContextMenu.h"
#include "CopyDialogWindow.xaml.h"
#pragma comment(lib, "gdiplus.lib")


namespace winrt::FastCopy::implementation
{
	winrt::FastCopy::RobocopyViewModel CopyDialog::ViewModel()
	{
		return ViewModelLocator::GetInstance().RobocopyViewModel();
	}

	void CopyDialog::HyperlinkButton_Click(
		winrt::Windows::Foundation::IInspectable const&, 
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		winrt::Windows::System::Launcher::LaunchUriAsync(ViewModel().DestinationUri());
	}


	void CopyDialog::PauseButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		if (PauseIcon().Symbol() == winrt::Microsoft::UI::Xaml::Controls::Symbol::Pause)
		{
			ViewModel().Pause();
			PauseIcon().Symbol(winrt::Microsoft::UI::Xaml::Controls::Symbol::Play);
			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(
				*this,
				L"PausedState",
				false
			);
		}
		else
		{
			ViewModel().Start();
			PauseIcon().Symbol(winrt::Microsoft::UI::Xaml::Controls::Symbol::Pause);
			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(
				*this,
				L"NormalState",
				false
			);
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
				if (IsLoaded())
				{
					applyTheme(e.theme);
					return;
				}

				Loaded([this, theme = e.theme](auto&&...) 
				{
					applyTheme(theme);
				});
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


	void CopyDialog::ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto copyWindow = winrt::get_self<CopyDialogWindow>(Global::copyWindow);
		if (m_showSpeedGraph)
		{
			HideGraphAnimation().Completed([this](auto...) { UnloadObject(SpeedGraph()); });
			HideGraphAnimation().Begin();
			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"HideGraph", false);
			copyWindow->Resize(m_originalWindowSize);
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
					SpeedGraph().SetSpeed(ViewModel().Percent() * 100.0, ViewModel().Speed());
				}
			});

			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"ShowGraph", false);
			m_originalWindowSize = copyWindow->ActualSize();
			copyWindow->Resize({ (int)m_originalWindowSize.Width, (int)m_originalWindowSize.Height + 190 });
		}
		m_showSpeedGraph = !m_showSpeedGraph;
	}

	void CopyDialog::ShowMenu(winrt::hstring path, winrt::Microsoft::UI::Xaml::Controls::MenuFlyout flyout)
	{
		std::make_shared<FileContextMenu>(path)->ShowAt(flyout);
	}

	void CopyDialog::applyTheme(int themeIndex)
	{
		switch (themeIndex)
		{
			case 0: return Parent().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Default);
			case 1: return Parent().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Light);
			case 2: return Parent().as<winrt::Microsoft::UI::Xaml::FrameworkElement>().RequestedTheme(winrt::Microsoft::UI::Xaml::ElementTheme::Dark);
		}
	}

	void CopyDialog::MenuFlyout_Opening(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const&)
	{
		auto menu = sender
			.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyout>();
		auto fileInfoViewModel = menu
			.GetValue(winrt::FastCopy::DataInjector::DataProperty())
			.as<winrt::FastCopy::FileInfoViewModel>();

		ShowMenu(fileInfoViewModel.Path(), menu);
	}

}
