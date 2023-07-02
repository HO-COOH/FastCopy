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
#include "PathUtils.h"
#include <ShlGuid.h>
#include <ShlObj_core.h>
#include <winrt/Windows.System.Threading.h>
#include "ImageUtils.h"
#include <Windows.h>
#include <gdiplus.h>
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

		winrt::com_ptr<IShellItem> item, folderItem;
		SHCreateItemFromParsingName(ToBackslash(path).data(), NULL, IID_PPV_ARGS(item.put()));

		item->GetParent(folderItem.put());

		winrt::com_ptr<IShellFolder> folder;
		folderItem->BindToHandler(nullptr, BHID_SFObject, IID_PPV_ARGS(folder.put()));


		LPITEMIDLIST child{ };

		SHGetIDListFromObject(item.get(), &child);
		auto last = ILFindLastID(child);

		LPCITEMIDLIST idl[2]{ last };

		winrt::com_ptr<IContextMenu> menu;
		auto hr = folder->GetUIObjectOf(NULL, 1, &idl[0], IID_IContextMenu, nullptr, menu.put_void());

		auto hMenu = CreatePopupMenu();
		//menu->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_OPTIMIZEFORINVOKE);
		menu->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_NORMAL);

		auto const itemCount = GetMenuItemCount(hMenu);
		MENUITEMINFO menuItemInfo{
			.cbSize = sizeof(menuItemInfo),
			.fMask = MIIM_BITMAP | MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_SUBMENU
		};
		for (int i = 0; i < itemCount; ++i)
		{
			TCHAR buffer[500]{};
			menuItemInfo.dwTypeData = buffer;
			menuItemInfo.cch = std::size(buffer);
			GetMenuItemInfo(hMenu, i, true, &menuItemInfo);
			if (!std::wstring_view{buffer}.empty() && menu)
			{
				winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem item;
				item.Text(buffer);

				if (menuItemInfo.hbmpItem)
				{
					winrt::Microsoft::UI::Xaml::Controls::ImageIcon icon;
					{
						auto pbitmap = Gdiplus::Bitmap::FromHBITMAP(menuItemInfo.hbmpItem, NULL);
						INT const height = pbitmap->GetHeight();
						INT const width = pbitmap->GetWidth();

						Gdiplus::Color c{Gdiplus::Color::MakeARGB(255, 0, 0, 0)};

						Gdiplus::Rect bmBounds{0, 0, (INT)width, (INT)height};
						Gdiplus::BitmapData data{};
						pbitmap->LockBits(&bmBounds, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &data);
						
						Gdiplus::Bitmap tmp{width, height, data.Stride, PixelFormat32bppARGB, (BYTE*)data.Scan0};
						Gdiplus::Bitmap clone{width, height, PixelFormat32bppARGB};

						Gdiplus::Graphics* gr = Gdiplus::Graphics::FromImage(&clone);
						gr->DrawImage(&tmp, bmBounds);

						HBITMAP hbitmap{};
						clone.GetHBITMAP(c, &hbitmap);
						if (auto source = HBitmapToWriteableBitmap(hbitmap); source != nullptr)
						{
							icon.Source(source);
							item.Icon(icon);
						}
					}
				}
				flyout.Items().Append(item);
			}
			OutputDebugString(buffer);
			OutputDebugString(L"\n");
		}

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

	//std::thread{
	//	[this, path = fileInfoViewModel.Path()]
	//	{
	//		winrt::init_apartment(winrt::apartment_type::single_threaded);
	//		ShowMenu(path, nullptr);
	//	}
	//}.detach();
}
