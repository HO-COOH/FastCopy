#include "pch.h"
#include "FileContextMenu.h"
#include "PathUtils.h"
#include <ShlGuid.h>
#include <ShlObj_core.h>
#include "ImageUtils.h"
#include <Windows.h>
#include <gdiplus.h>

void FileContextMenu::ShowAt(winrt::Microsoft::UI::Xaml::Controls::MenuFlyout& flyout)
{
	winrt::com_ptr<IShellItem> item, folderItem;
	SHCreateItemFromParsingName(m_path.data(), NULL, IID_PPV_ARGS(item.put()));

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
