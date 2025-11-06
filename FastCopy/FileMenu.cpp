#include "pch.h"
#include "FileMenu.h"
#if __has_include("FileMenu.g.cpp")
#include "FileMenu.g.cpp"
#endif
#include "FileInfoViewModel.h"
#include "GdiplusInitializer.h"
#include <boost/algorithm/string.hpp>
#include <ShlGuid.h>
#include <ShlObj_core.h>
#include <wil/resource.h>
#include "ImageUtils.h"
#include "CopyDialog.h"

static winrt::Microsoft::UI::Xaml::Media::ImageSource getIconFromWin32Menu(HBITMAP menuItemInfoBitmap)
{
	static GdiplusInitializer s_gdiPlusInitializer;
	if (!menuItemInfoBitmap)
		return nullptr;

	auto pbitmap = Gdiplus::Bitmap::FromHBITMAP(menuItemInfoBitmap, NULL);
	INT const height = pbitmap->GetHeight();
	INT const width = pbitmap->GetWidth();

	Gdiplus::Color c{ Gdiplus::Color::MakeARGB(255, 0, 0, 0) };

	Gdiplus::Rect bmBounds{ 0, 0, (INT)width, (INT)height };
	Gdiplus::BitmapData data{};
	pbitmap->LockBits(&bmBounds, Gdiplus::ImageLockModeRead, PixelFormat32bppRGB, &data);

	Gdiplus::Bitmap tmp{ width, height, data.Stride, PixelFormat32bppARGB, (BYTE*)data.Scan0 };
	Gdiplus::Bitmap clone{ width, height, PixelFormat32bppARGB };

	Gdiplus::Graphics* gr = Gdiplus::Graphics::FromImage(&clone);
	gr->DrawImage(&tmp, bmBounds);

	HBITMAP hbitmap{};
	clone.GetHBITMAP(c, &hbitmap);
	return HBitmapToWriteableBitmap(hbitmap);
}

static winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator getKeyboardAccelerator(std::wstring& menuText)
{
	auto const index = menuText.rfind(L"&");
	if (index == std::wstring::npos)
		return nullptr;

	//auto const endIndex = menuText.find(L')', index + 1);
	if (/*endIndex == std::wstring::npos*/menuText[index + 2] != L')')
	{
		boost::replace_all(menuText, L"&", L"");
		return nullptr;
	}

	auto keyboardAcceleratorText = menuText.substr(index + 1, 1);
	assert(keyboardAcceleratorText.size() == 1);
	winrt::Microsoft::UI::Xaml::Input::KeyboardAccelerator result;
	result.Key(static_cast<winrt::Windows::System::VirtualKey>(keyboardAcceleratorText[0]));
	boost::replace_all(menuText, L"&", L"");
	menuText.erase(menuText.end() - 3, menuText.end());
	return result;
}


namespace winrt::FastCopy::implementation
{
	winrt::FastCopy::FileInfoViewModel FileMenu::File()
	{
		return m_fileInfo;
	}

	void FileMenu::File(winrt::FastCopy::FileInfoViewModel const& value)
	{
		m_fileInfo = value;
		m_fileInfoImpl = winrt::get_self<FileInfoViewModel>(m_fileInfo);


		auto flyoutItems = Items();
		if (flyoutItems.Size() != 0)
			return;

		winrt::com_ptr<IShellItem> item = CreateItemFromParsingName(m_fileInfoImpl->Path().data()), folderItem;
		item->GetParent(folderItem.put());

		winrt::com_ptr<IShellFolder> folder;
		folderItem->BindToHandler(nullptr, BHID_SFObject, IID_PPV_ARGS(folder.put()));


		LPITEMIDLIST child{ };

		SHGetIDListFromObject(item.get(), &child);
		auto last = ILFindLastID(child);

		LPCITEMIDLIST idl[2]{ last };

		auto hr = folder->GetUIObjectOf(NULL, 1, &idl[0], IID_IContextMenu, nullptr, m_menu.put_void());

		wil::unique_hmenu hMenu{ CreatePopupMenu() };
		m_menu->QueryContextMenu(hMenu.get(), 0, 1, 0x7fff, CMF_NORMAL);



		std::vector<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase> menuItems;
		menuFlyoutFromHMenu(menuItems, hMenu.get());

		flyoutItems.ReplaceAll(menuItems);
	}

	winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem FileMenu::makeMenuFlyout(MENUITEMINFO const& menuItemInfo)
	{
		winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem item;
		std::wstring buf{ menuItemInfo.dwTypeData };
		
		if (auto keyboardAccelerator = getKeyboardAccelerator(buf))
			item.KeyboardAccelerators().Append(keyboardAccelerator);

		item.Text(std::move(buf)); //dwTypeData has additional '&' that needs to be removed, see Files ShellContextMenuHelper.cs LoadMenuFlyoutItem() function


		item.Click([this](auto self, ...) {
			int const id = winrt::unbox_value<decltype(m_menuData.size())>(self.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tag());
			auto const& data = m_menuData[id];
			CMINVOKECOMMANDINFOEX invokeInfo
			{
				.cbSize = sizeof(invokeInfo),
				.lpVerb = reinterpret_cast<decltype(invokeInfo.lpVerb)>(MAKEINTRESOURCEW(data.menuId)),
				.nShow = SW_SHOWNORMAL,
			};
			m_menu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&invokeInfo));
			});

		if (auto source = getIconFromWin32Menu(menuItemInfo.hbmpItem))
		{
			winrt::Microsoft::UI::Xaml::Controls::ImageIcon icon;
			icon.Source(source);
			item.Icon(icon);
		}
		item.Tag(winrt::box_value(m_menuData.size()));
		return item;
	}

	void FileMenu::menuFlyoutFromHMenu(std::vector<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase>& items, HMENU menu)
	{
		auto const itemCount = GetMenuItemCount(menu);
		items.reserve(items.size() + itemCount);

		MENUITEMINFO menuItemInfo
		{
			.cbSize = sizeof(menuItemInfo),
			.fMask = MIIM_BITMAP | MIIM_FTYPE | MIIM_STRING | MIIM_ID | MIIM_SUBMENU
		};

		bool hasSubItem = false;

		for (int i = 0; i < itemCount; ++i)
		{
			TCHAR buffer[500]{};
			menuItemInfo.dwTypeData = buffer;
			menuItemInfo.cch = std::size(buffer);
			GetMenuItemInfo(
				menu,	//hmenu
				i,				//item
				true,			//by position
				&menuItemInfo	//menuItemInfo
			);

			if (menuItemInfo.fType & MFT_SEPARATOR)
			{
				items.push_back(winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSeparator{});
				continue;
			}

			std::wstring_view itemName{ buffer };
			if (itemName == L"FastCopy")
				continue;

			if (!itemName.empty() && m_menu)
			{
				if (menuItemInfo.hSubMenu)
				{
					hasSubItem = true;
					winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem subItem;
					std::wstring subItemText{ buffer };
					if (auto keyboardAccelerator = getKeyboardAccelerator(subItemText))
						subItem.KeyboardAccelerators().Append(keyboardAccelerator);
					subItem.Text(std::move(subItemText));
					std::vector<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase> subItems;
					menuFlyoutFromHMenu(subItems, menuItemInfo.hSubMenu);
					subItem.Items().ReplaceAll(subItems);
					items.push_back(subItem);
					continue;
				}
				if (auto flyoutItem = makeMenuFlyout(menuItemInfo))
					items.push_back(flyoutItem);
				std::wstring command(100, {});
				m_menu->GetCommandString(menuItemInfo.wID - 1, GCS_VERB, nullptr, reinterpret_cast<char*>(command.data()), command.size());
				m_menuData.push_back(MenuData{ .menuId = menuItemInfo.wID - 1, .verb = std::move(command) });
			}
		}

		//set style
		for (auto& flyoutItem : items)
		{
			if (auto normalItem = flyoutItem.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>(); normalItem && normalItem.KeyboardAccelerators().Size() != 0)
				normalItem.Style(hasSubItem ? CopyDialog::MenuFlyoutItemHasSubItemStyle : CopyDialog::MenuFlyoutItemNoSubItemStyle);
			else if (auto subItem = flyoutItem.try_as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutSubItem>(); subItem && hasSubItem && subItem.KeyboardAccelerators().Size() != 0)
				subItem.Style(CopyDialog::MenuFlyoutSubItemStyle);
		}
	}
}
