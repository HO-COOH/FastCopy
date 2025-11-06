#include "pch.h"
#include "CopyDialog.h"
#if __has_include("CopyDialog.g.cpp")
#include "CopyDialog.g.cpp"
#endif

#include "ViewModelLocator.h"
#include <winrt/Windows.System.h>
#include "Global.h"
#include "SettingsChangeListener.h"
#include "CopyDialogWindow.xaml.h"
#pragma comment(lib, "gdiplus.lib")


namespace winrt::FastCopy::implementation
{
	winrt::Microsoft::UI::Xaml::Style CopyDialog::MenuFlyoutItemNoSubItemStyle{ nullptr };
	winrt::Microsoft::UI::Xaml::Style CopyDialog::MenuFlyoutItemHasSubItemStyle{ nullptr };
	winrt::Microsoft::UI::Xaml::Style CopyDialog::MenuFlyoutSubItemStyle{ nullptr };
	
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

	CopyDialog::CopyDialog()
	{
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

		InitializeComponent();
		MenuFlyoutItemNoSubItemStyle = MyMenuFlyoutItemNoSubItemStyle();
		MenuFlyoutItemHasSubItemStyle = MyMenuFlyoutItemHasSubItemStyle();
		MenuFlyoutSubItemStyle = MyMenuFlyoutSubItemStyle();
	}


	void CopyDialog::ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto copyWindow = winrt::get_self<CopyDialogWindow>(Global::copyWindow);
		if (m_showSpeedGraph)
		{
			HideGraphAnimation().Completed([this](auto...) 
				{
					SpeedGraph().Margin({});
					SpeedGraph().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed); 
				});
			HideGraphAnimation().Begin();
			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"HideGraph", false);
			copyWindow->Resize(m_originalWindowSize);
			//ViewModel().PropertyChanged(m_speedUpdateRevoker);
		}
		else
		{
			if (m_speedGraphLoaded)
			{
				SpeedGraph().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
				SpeedGraph().Margin({ 0.0, 20.0, 0.0, 3.0 });
				GraphAnimation().Begin();
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
			}

			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(*this, L"ShowGraph", false);
			m_originalWindowSize = copyWindow->ActualSize();
			copyWindow->Resize({ (int)m_originalWindowSize.Width, (int)m_originalWindowSize.Height + 190 });
			m_speedGraphLoaded = true;
		}
		m_showSpeedGraph = !m_showSpeedGraph;
	}

	constexpr auto modifierToText(winrt::Windows::System::VirtualKeyModifiers modifier)
	{
		switch (modifier)
		{
			case winrt::Windows::System::VirtualKeyModifiers::Control: return L"Ctrl";
			case winrt::Windows::System::VirtualKeyModifiers::Menu: return L"Alt";
			case winrt::Windows::System::VirtualKeyModifiers::Shift: return L"Shift";
			case winrt::Windows::System::VirtualKeyModifiers::Windows: return L"Win";
			default: return L"";
		}
	}

	constexpr auto virtualKeyToText(winrt::Windows::System::VirtualKey key)
	{
		switch (key)
		{
			case winrt::Windows::System::VirtualKey::None:
			case winrt::Windows::System::VirtualKey::LeftButton:
			case winrt::Windows::System::VirtualKey::RightButton:
			case winrt::Windows::System::VirtualKey::Cancel:
			case winrt::Windows::System::VirtualKey::MiddleButton:
			case winrt::Windows::System::VirtualKey::XButton1:
			case winrt::Windows::System::VirtualKey::XButton2:
			case winrt::Windows::System::VirtualKey::Back:
			case winrt::Windows::System::VirtualKey::Tab:
			case winrt::Windows::System::VirtualKey::Clear:
			case winrt::Windows::System::VirtualKey::Enter:
			case winrt::Windows::System::VirtualKey::Shift:
			case winrt::Windows::System::VirtualKey::Control:
			case winrt::Windows::System::VirtualKey::Menu:
			case winrt::Windows::System::VirtualKey::Pause:
			case winrt::Windows::System::VirtualKey::CapitalLock:
			case winrt::Windows::System::VirtualKey::Kana:
			//case winrt::Windows::System::VirtualKey::Hangul:
			case winrt::Windows::System::VirtualKey::ImeOn:
			case winrt::Windows::System::VirtualKey::Junja:
			case winrt::Windows::System::VirtualKey::Final:
			case winrt::Windows::System::VirtualKey::Hanja:
			//case winrt::Windows::System::VirtualKey::Kanji:
			case winrt::Windows::System::VirtualKey::ImeOff:
			case winrt::Windows::System::VirtualKey::Escape:
			case winrt::Windows::System::VirtualKey::Convert:
			case winrt::Windows::System::VirtualKey::NonConvert:
			case winrt::Windows::System::VirtualKey::Accept:
			case winrt::Windows::System::VirtualKey::ModeChange:
			case winrt::Windows::System::VirtualKey::Space:
			case winrt::Windows::System::VirtualKey::PageUp:
			case winrt::Windows::System::VirtualKey::PageDown:
			case winrt::Windows::System::VirtualKey::End:
			case winrt::Windows::System::VirtualKey::Home:
			case winrt::Windows::System::VirtualKey::Left:
			case winrt::Windows::System::VirtualKey::Up:
			case winrt::Windows::System::VirtualKey::Right:
			case winrt::Windows::System::VirtualKey::Down:
			case winrt::Windows::System::VirtualKey::Select:
			case winrt::Windows::System::VirtualKey::Print:
			case winrt::Windows::System::VirtualKey::Execute:
			case winrt::Windows::System::VirtualKey::Snapshot:
			case winrt::Windows::System::VirtualKey::Insert:
			case winrt::Windows::System::VirtualKey::Delete:
			case winrt::Windows::System::VirtualKey::Help:
			case winrt::Windows::System::VirtualKey::Number0:
			case winrt::Windows::System::VirtualKey::Number1:
			case winrt::Windows::System::VirtualKey::Number2:
			case winrt::Windows::System::VirtualKey::Number3:
			case winrt::Windows::System::VirtualKey::Number4:
			case winrt::Windows::System::VirtualKey::Number5:
			case winrt::Windows::System::VirtualKey::Number6:
			case winrt::Windows::System::VirtualKey::Number7:
			case winrt::Windows::System::VirtualKey::Number8:
			case winrt::Windows::System::VirtualKey::Number9:
				assert(false);
			case winrt::Windows::System::VirtualKey::A: return L"A";
			case winrt::Windows::System::VirtualKey::B: return L"B";
			case winrt::Windows::System::VirtualKey::C: return L"C";
			case winrt::Windows::System::VirtualKey::D: return L"D";
			case winrt::Windows::System::VirtualKey::E: return L"E";
			case winrt::Windows::System::VirtualKey::F: return L"F";
			case winrt::Windows::System::VirtualKey::G: return L"G";
			case winrt::Windows::System::VirtualKey::H: return L"H";
			case winrt::Windows::System::VirtualKey::I: return L"I";
			case winrt::Windows::System::VirtualKey::J: return L"J";
			case winrt::Windows::System::VirtualKey::K: return L"K";
			case winrt::Windows::System::VirtualKey::L: return L"L";
			case winrt::Windows::System::VirtualKey::M: return L"M";
			case winrt::Windows::System::VirtualKey::N: return L"N";
			case winrt::Windows::System::VirtualKey::O: return L"O";
			case winrt::Windows::System::VirtualKey::P: return L"P";
			case winrt::Windows::System::VirtualKey::Q: return L"Q";
			case winrt::Windows::System::VirtualKey::R: return L"R";
			case winrt::Windows::System::VirtualKey::S: return L"S";
			case winrt::Windows::System::VirtualKey::T: return L"T";
			case winrt::Windows::System::VirtualKey::U: return L"U";
			case winrt::Windows::System::VirtualKey::V: return L"V";
			case winrt::Windows::System::VirtualKey::W: return L"W";
			case winrt::Windows::System::VirtualKey::X: return L"X";
			case winrt::Windows::System::VirtualKey::Y: return L"Y";
			case winrt::Windows::System::VirtualKey::Z: return L"Z";

			case winrt::Windows::System::VirtualKey::LeftWindows:
			case winrt::Windows::System::VirtualKey::RightWindows:
			case winrt::Windows::System::VirtualKey::Application:
			case winrt::Windows::System::VirtualKey::Sleep:
				assert(false);
			case winrt::Windows::System::VirtualKey::NumberPad0: return L"0";
			case winrt::Windows::System::VirtualKey::NumberPad1: return L"1";
			case winrt::Windows::System::VirtualKey::NumberPad2: return L"2";
			case winrt::Windows::System::VirtualKey::NumberPad3: return L"3";
			case winrt::Windows::System::VirtualKey::NumberPad4: return L"4";
			case winrt::Windows::System::VirtualKey::NumberPad5: return L"5";
			case winrt::Windows::System::VirtualKey::NumberPad6: return L"6";
			case winrt::Windows::System::VirtualKey::NumberPad7: return L"7";
			case winrt::Windows::System::VirtualKey::NumberPad8: return L"8";
			case winrt::Windows::System::VirtualKey::NumberPad9: return L"9";

			case winrt::Windows::System::VirtualKey::Multiply:
			case winrt::Windows::System::VirtualKey::Add:
			case winrt::Windows::System::VirtualKey::Separator:
			case winrt::Windows::System::VirtualKey::Subtract:
			case winrt::Windows::System::VirtualKey::Decimal:
			case winrt::Windows::System::VirtualKey::Divide:
			case winrt::Windows::System::VirtualKey::F1:
			case winrt::Windows::System::VirtualKey::F2:
			case winrt::Windows::System::VirtualKey::F3:
			case winrt::Windows::System::VirtualKey::F4:
			case winrt::Windows::System::VirtualKey::F5:
			case winrt::Windows::System::VirtualKey::F6:
			case winrt::Windows::System::VirtualKey::F7:
			case winrt::Windows::System::VirtualKey::F8:
			case winrt::Windows::System::VirtualKey::F9:
			case winrt::Windows::System::VirtualKey::F10:
			case winrt::Windows::System::VirtualKey::F11:
			case winrt::Windows::System::VirtualKey::F12:
			case winrt::Windows::System::VirtualKey::F13:
			case winrt::Windows::System::VirtualKey::F14:
			case winrt::Windows::System::VirtualKey::F15:
			case winrt::Windows::System::VirtualKey::F16:
			case winrt::Windows::System::VirtualKey::F17:
			case winrt::Windows::System::VirtualKey::F18:
			case winrt::Windows::System::VirtualKey::F19:
			case winrt::Windows::System::VirtualKey::F20:
			case winrt::Windows::System::VirtualKey::F21:
			case winrt::Windows::System::VirtualKey::F22:
			case winrt::Windows::System::VirtualKey::F23:
			case winrt::Windows::System::VirtualKey::F24:
			case winrt::Windows::System::VirtualKey::NavigationView:
			case winrt::Windows::System::VirtualKey::NavigationMenu:
			case winrt::Windows::System::VirtualKey::NavigationUp:
			case winrt::Windows::System::VirtualKey::NavigationDown:
			case winrt::Windows::System::VirtualKey::NavigationLeft:
			case winrt::Windows::System::VirtualKey::NavigationRight:
			case winrt::Windows::System::VirtualKey::NavigationAccept:
			case winrt::Windows::System::VirtualKey::NavigationCancel:
			case winrt::Windows::System::VirtualKey::NumberKeyLock:
			case winrt::Windows::System::VirtualKey::Scroll:
			case winrt::Windows::System::VirtualKey::LeftShift:
			case winrt::Windows::System::VirtualKey::RightShift:
			case winrt::Windows::System::VirtualKey::LeftControl:
			case winrt::Windows::System::VirtualKey::RightControl:
			case winrt::Windows::System::VirtualKey::LeftMenu:
			case winrt::Windows::System::VirtualKey::RightMenu:
			case winrt::Windows::System::VirtualKey::GoBack:
			case winrt::Windows::System::VirtualKey::GoForward:
			case winrt::Windows::System::VirtualKey::Refresh:
			case winrt::Windows::System::VirtualKey::Stop:
			case winrt::Windows::System::VirtualKey::Search:
			case winrt::Windows::System::VirtualKey::Favorites:
			case winrt::Windows::System::VirtualKey::GoHome:
			case winrt::Windows::System::VirtualKey::GamepadA:
			case winrt::Windows::System::VirtualKey::GamepadB:
			case winrt::Windows::System::VirtualKey::GamepadX:
			case winrt::Windows::System::VirtualKey::GamepadY:
			case winrt::Windows::System::VirtualKey::GamepadRightShoulder:
			case winrt::Windows::System::VirtualKey::GamepadLeftShoulder:
			case winrt::Windows::System::VirtualKey::GamepadLeftTrigger:
			case winrt::Windows::System::VirtualKey::GamepadRightTrigger:
			case winrt::Windows::System::VirtualKey::GamepadDPadUp:
			case winrt::Windows::System::VirtualKey::GamepadDPadDown:
			case winrt::Windows::System::VirtualKey::GamepadDPadLeft:
			case winrt::Windows::System::VirtualKey::GamepadDPadRight:
			case winrt::Windows::System::VirtualKey::GamepadMenu:
			case winrt::Windows::System::VirtualKey::GamepadView:
			case winrt::Windows::System::VirtualKey::GamepadLeftThumbstickButton:
			case winrt::Windows::System::VirtualKey::GamepadRightThumbstickButton:
			case winrt::Windows::System::VirtualKey::GamepadLeftThumbstickUp:
			case winrt::Windows::System::VirtualKey::GamepadLeftThumbstickDown:
			case winrt::Windows::System::VirtualKey::GamepadLeftThumbstickRight:
			case winrt::Windows::System::VirtualKey::GamepadLeftThumbstickLeft:
			case winrt::Windows::System::VirtualKey::GamepadRightThumbstickUp:
			case winrt::Windows::System::VirtualKey::GamepadRightThumbstickDown:
			case winrt::Windows::System::VirtualKey::GamepadRightThumbstickRight:
			case winrt::Windows::System::VirtualKey::GamepadRightThumbstickLeft:
			default:
				assert(false);
		}
	}

	winrt::hstring CopyDialog::GetTextFromKeyboardAccelerators(winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator> const& keys)
	{
		if (keys.Size() == 0)
			return {};

		auto key = *keys.First();
		auto const modifiers = key.Modifiers();
		if (modifiers == winrt::Windows::System::VirtualKeyModifiers::None)
			return virtualKeyToText(key.Key());
		return winrt::hstring{ std::format(L"{}+{}", modifierToText(modifiers), virtualKeyToText(key.Key())) };
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

}
