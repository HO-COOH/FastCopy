#include "pch.h"
#include "FileContextMenu.h"
#include "PathUtils.h"
#include <ShlGuid.h>
#include <ShlObj_core.h>
#include "ImageUtils.h"
#include <Windows.h>
#include <gdiplus.h>
#include <boost/algorithm/string.hpp>

class GdiplusInitializer
{
public:
	GdiplusInitializer()
	{
		Gdiplus::GdiplusStartup(&gdiplusToken, &input, nullptr);
	}

	~GdiplusInitializer()
	{
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
private:
	Gdiplus::GdiplusStartupInput input;
	ULONG_PTR gdiplusToken;
};

winrt::Microsoft::UI::Xaml::Media::ImageSource FileContextMenu::getIconFromWin32Menu(HBITMAP menuItemInfoBitmap)
{
	static GdiplusInitializer s_gdiPlusInitializer;
	if (!menuItemInfoBitmap)
		return nullptr;

	auto pbitmap = Gdiplus::Bitmap::FromHBITMAP(menuItemInfoBitmap, NULL);
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
	return HBitmapToWriteableBitmap(hbitmap);
}

winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem FileContextMenu::makeMenuFlyout(MENUITEMINFO const& menuItemInfo)
{
	winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem item;
	std::wstring buf{menuItemInfo.dwTypeData};
	boost::replace_all(buf, L"&", L"");
	item.Text(std::move(buf)); //dwTypeData has additional '&' that needs to be removed, see Files ShellContextMenuHelper.cs LoadMenuFlyoutItem() function
	

	item.Click([thisPtr = shared_from_this()](auto self, ...) {
		int const id = winrt::unbox_value<decltype(m_menuData.size())>(self.as<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem>().Tag());
		auto const& data = thisPtr->m_menuData[id];
		CMINVOKECOMMANDINFOEX invokeInfo
		{
			.cbSize = sizeof(invokeInfo),
			.lpVerb = reinterpret_cast<decltype(invokeInfo.lpVerb)>(MAKEINTRESOURCEW(data.menuId)),
			.nShow = SW_SHOWNORMAL,
		};
		thisPtr->m_menu->InvokeCommand(reinterpret_cast<CMINVOKECOMMANDINFO*>(&invokeInfo));
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

void FileContextMenu::ShowAt(winrt::Microsoft::UI::Xaml::Controls::MenuFlyout& flyout)
{
	if (flyout.Items().Size() != 0)
		return;

	winrt::com_ptr<IShellItem> item = CreateItemFromParsingName(m_path.data()), folderItem;
	item->GetParent(folderItem.put());

	winrt::com_ptr<IShellFolder> folder;
	folderItem->BindToHandler(nullptr, BHID_SFObject, IID_PPV_ARGS(folder.put()));


	LPITEMIDLIST child{ };

	SHGetIDListFromObject(item.get(), &child);
	auto last = ILFindLastID(child);

	LPCITEMIDLIST idl[2]{ last };

	auto hr = folder->GetUIObjectOf(NULL, 1, &idl[0], IID_IContextMenu, nullptr, m_menu.put_void());

	auto hMenu = CreatePopupMenu();
	m_menu->QueryContextMenu(hMenu, 0, 1, 0x7fff, CMF_NORMAL);

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
		if (!std::wstring_view{buffer}.empty() && m_menu)
		{
			flyout.Items().Append(makeMenuFlyout(menuItemInfo));
			std::wstring command(100, {});
			m_menu->GetCommandString(menuItemInfo.wID - 1, GCS_VERB, nullptr, reinterpret_cast<char*>(command.data()), command.size());
			m_menuData.push_back(MenuData{ .menuId = menuItemInfo.wID - 1, .verb = std::move(command) });
		}
	}
}
