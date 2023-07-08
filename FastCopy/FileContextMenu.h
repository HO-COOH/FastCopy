#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <vector>
#include "PathUtils.h"
#include <memory>

struct MenuData
{

	unsigned int menuId;
	std::wstring verb;
};

class FileContextMenu : public std::enable_shared_from_this<FileContextMenu>
{
	std::vector<MenuData> m_menuData;
	std::wstring m_path;
	void onMenuItemClick() {}
	winrt::com_ptr<IContextMenu> m_menu;

	static winrt::Microsoft::UI::Xaml::Media::ImageSource getIconFromWin32Menu(HBITMAP menuItemInfoBitmap);
	winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem makeMenuFlyout(MENUITEMINFO const& menuItemInfo);
public:
	FileContextMenu(winrt::hstring path) : m_path{ ToBackslash(path)} {}

	void ShowAt(winrt::Microsoft::UI::Xaml::Controls::MenuFlyout& flyout);
};
