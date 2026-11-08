// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "CopyDialogWindow.xaml.h"
#if __has_include("CopyDialogWindow.g.cpp")
#include "CopyDialogWindow.g.cpp"
#endif

#include "WindowHelper.h"
#include "Global.h"
#include "Taskbar.h"
#include "ViewModelLocator.h"
#include <include/HwndHelper.hpp>
#include <winrt/Windows.System.h>
#include "WindowEffectHelper.h"
#pragma comment(lib, "gdiplus.lib")

namespace winrt::FastCopy::implementation
{
	CopyDialogWindow::CopyDialogWindow(WindowEffectHelper& windowEffectHelper) : m_windowEffectHelper{ windowEffectHelper }
    {
        //Don't touch Window()/Hwnd()/AppWindow()/SetTitleBar() here, they make us the window content.
        //Parsing the XAML while already in the window's namescope puts the x:Load stubs in that namescope instead of ours, and FindName can't find them
        Global::UIThread = DispatcherQueue();

        //The view model outlives this window, so its handlers hold it weakly. A strong ref would keep it alive past XAML shutdown
        ViewModel().DuplicateFiles().VectorChanged(
            { get_weak(), [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto&&)
            {
                auto const numElements = original.Size();
                auto const clamped = std::clamp<uint32_t>(numElements, 0, std::size(Sizes) - 1);

                //set size cache and play window animation
                //playWindowAnimation(m_currentSize = Sizes[clamped]);

				if (numElements == 0)
				{
					UnloadObject(DuplicateFileInfoGrid());
					UnloadObject(ContinueButton());
				}
				else
				{
					DispatcherQueue().TryEnqueue({ get_weak(), [this]
					{
						FindName(L"ContinueButton");
						FindName(L"DuplicateFileItems");
						FindName(L"DuplicateFileInfoGrid");
					} });
				}

				winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(
					VisualStateRoot(),
					numElements == 0 ? L"HideDuplicateFiles" : L"ShowDuplicateFiles",
					false
				);
            } }
        );
        ViewModel().Finished(
            [weak = get_weak()](winrt::Windows::Foundation::IInspectable const&, winrt::FastCopy::FinishState state)
            {
                if (state != winrt::FastCopy::FinishState::Failed)
                    return;

                //Some items failed: hide the progress dialog and show the (bound) error text instead
                //This is raised off the UI thread, and resolving there could run our destructor there, so wait until we're on it
                Global::UIThread.TryEnqueue([weak]
                {
                    auto self = weak.get();
                    if (!self)
                        return;

                    self->CopyDialog().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
                    self->ErrorScrollViewer().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
                    //self->playWindowAnimation(Sizes[std::size(Sizes) - 1]);
                });
            }
        );
        //Other properties can be raised off the UI thread, and resolving there could run our destructor there. Speed is raised on the UI thread
        ViewModel().PropertyChanged(
            [weak = get_weak()](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& e)
            {
                if (e.PropertyName() != L"Speed")
                    return;

                auto self = weak.get();
                if (!self)
                    return;

                //SpeedGraph is x:Load'ed by the ShowGraph visual state, it's null until the graph is first shown
                if (auto const speedGraph = self->SpeedGraph())
                    speedGraph.SetSpeed(self->ViewModel().Percent() * 100.0, self->ViewModel().Speed());
            }
        );
    }

    void CopyDialogWindow::InitializeComponent()
    {
		CopyDialogWindowT::InitializeComponent();
        CenterWindow(Window(), NormalSize);
        MenuFlyoutItemNoSubItemStyle = MyMenuFlyoutItemNoSubItemStyle();
        MenuFlyoutItemHasSubItemStyle = MyMenuFlyoutItemHasSubItemStyle();
        MenuFlyoutSubItemStyle = MyMenuFlyoutSubItemStyle();
		m_windowEffectHelper.SetTarget(*this, RootPanel());
		m_windowEffectHelper.SetListenThemeChange();
		BringToForeground(reinterpret_cast<HWND>(Hwnd()));
    }

    winrt::FastCopy::RobocopyViewModel CopyDialogWindow::ViewModel()
    {
        return ViewModelLocator::GetInstance().RobocopyViewModel();
    }

    void CopyDialogWindow::HyperlinkButton_Click(
        winrt::Windows::Foundation::IInspectable const&,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        winrt::Windows::System::Launcher::LaunchUriAsync(ViewModel().DestinationUri());
    }

    void CopyDialogWindow::ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_showSpeedGraph = !m_showSpeedGraph;
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(VisualStateRoot(), m_showSpeedGraph ? L"ShowGraph" : L"HideGraph", false);
    }

    void CopyDialogWindow::WindowEx_Closed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args)
    {
        ViewModelLocator::GetInstance().RobocopyViewModel().Cancel();
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

	winrt::hstring CopyDialogWindow::GetTextFromKeyboardAccelerators(winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator> const& keys)
	{
		if (keys.Size() == 0)
			return {};

		auto key = *keys.First();
		auto const modifiers = key.Modifiers();
		if (modifiers == winrt::Windows::System::VirtualKeyModifiers::None)
			return virtualKeyToText(key.Key());
		return winrt::hstring{ std::format(L"{}+{}", modifierToText(modifiers), virtualKeyToText(key.Key())) };
	}

}
