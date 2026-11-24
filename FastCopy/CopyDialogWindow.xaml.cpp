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
#include <include/HwndHelper.hpp>
#include <winrt/Windows.System.h>
#include "WindowEffectHelper.h"
#include "RobocopyViewModel.h"
#pragma comment(lib, "gdiplus.lib")

template<>
struct std::formatter<winrt::Windows::System::VirtualKeyModifiers, wchar_t> : std::formatter<std::wstring_view, wchar_t>
{
	constexpr static auto modifierToText(winrt::Windows::System::VirtualKeyModifiers modifier)
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

	auto format(winrt::Windows::System::VirtualKeyModifiers const& modifier, auto& ctx) const
	{
		return std::formatter<std::wstring_view, wchar_t>::format(modifierToText(modifier), ctx);
	}
};

template<>
struct std::formatter<winrt::Windows::System::VirtualKey, wchar_t> : std::formatter<std::wstring_view, wchar_t>
{
	constexpr static auto virtualKeyToText(winrt::Windows::System::VirtualKey key)
	{
		switch (key)
		{
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
			default:
				assert(false);
				return L"";
		}
	}

	auto format(winrt::Windows::System::VirtualKey const& key, auto& ctx) const
	{
		return std::formatter<std::wstring_view, wchar_t>::format(virtualKeyToText(key), ctx);
	}
};

static auto backSlashToForwardSlash(std::wstring_view path)
{
	std::wstring transformed;
	std::transform(path.begin(), path.end(), std::back_inserter(transformed), [](wchar_t c)
	{
		return c == L'\\' ? L'/' : c;
	});
	return transformed;
}


static winrt::Windows::Foundation::Uri pathToUri(std::wstring_view path)
{
	return winrt::Windows::Foundation::Uri{
		std::format(L"file:///{}", backSlashToForwardSlash(path))
	};
}

namespace winrt::FastCopy::implementation
{
	CopyDialogWindow::CopyDialogWindow(WindowEffectHelper& windowEffectHelper, RobocopyViewModel& robocopyViewModel) : 
		m_windowEffectHelper{ windowEffectHelper },
		m_viewModel{ robocopyViewModel }
    {
        //Don't touch Window()/Hwnd()/AppWindow()/SetTitleBar() here, they make us the window content.
        //Parsing the XAML while already in the window's namescope puts the x:Load stubs in that namescope instead of ours, and FindName can't find them
        Global::UIThread = DispatcherQueue();

        //The view model outlives this window, so its handlers hold it weakly. A strong ref would keep it alive past XAML shutdown
		m_viewModel.DuplicateFiles().VectorChanged(
            { get_weak(), [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto&&)
            {
				if (original.Size() == 0)
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

				updateVisualStates();
            } }
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
		return m_viewModel;
    }

    void CopyDialogWindow::HyperlinkButton_Click(
        winrt::Windows::Foundation::IInspectable const&,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        winrt::Windows::System::Launcher::LaunchUriAsync(pathToUri(m_viewModel.m_destination));
    }

    void CopyDialogWindow::ShowGraphButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
    {
        m_showSpeedGraph = !m_showSpeedGraph;
        updateVisualStates();
    }

    void CopyDialogWindow::updateVisualStates()
    {
        if (m_viewModel.State() == FastCopy::TaskbarState::Failed)
            return;

        //Matching Sizes[]
        constexpr wchar_t const* duplicateWindowSizeStates[]{ L"OneDuplicateWindowSize", L"TwoDuplicatesWindowSize", L"ManyDuplicatesWindowSize" };
        static_assert(std::size(duplicateWindowSizeStates) == std::size(Sizes));

        auto const duplicateCount = ViewModel().DuplicateFiles().Size();
		winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(VisualStateRoot(), duplicateCount == 0 ? L"HideDuplicateFiles" : L"ShowDuplicateFiles", false);

        //HideGraph targets SpeedGraph, which would realize its x:Load only to hide it
		auto const showGraph = m_showSpeedGraph && duplicateCount == 0;
        if (showGraph || SpeedGraph())
			winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(VisualStateRoot(), showGraph ? L"ShowGraph" : L"HideGraph", false);

		//window size states
        winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(
            VisualStateRoot(),
            duplicateCount != 0 ? duplicateWindowSizeStates[(std::min<size_t>)(duplicateCount, std::size(Sizes)) - 1] :
                showGraph ? L"GraphWindowSize" :
                L"NormalWindowSize",
            false
        );
    }

    void CopyDialogWindow::WindowEx_Closed(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::WindowEventArgs const&)
    {
        m_viewModel.Cancel();
    }

	winrt::hstring CopyDialogWindow::GetTextFromKeyboardAccelerators(winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator> const& keys)
	{
		if (keys.Size() == 0)
			return {};

		auto key = *keys.First();
		auto const virtualKey = key.Key();
		auto const modifiers = key.Modifiers();
		if (modifiers == winrt::Windows::System::VirtualKeyModifiers::None)
			return std::formatter<winrt::Windows::System::VirtualKey, wchar_t>::virtualKeyToText(virtualKey);

		return winrt::hstring{ std::format(L"{}+{}", modifiers, virtualKey) };
	}
}
