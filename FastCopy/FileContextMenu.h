#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <vector>
#include "PathUtils.h"

struct MenuData
{
	int menuId;
	std::wstring verb;
};

class FileContextMenu
{
	std::vector<MenuData> m_menuData;
	std::wstring m_path;
	void onMenuItemClick() {}
public:
	FileContextMenu(winrt::hstring path) : m_path{ ToBackslash(path)} {}

	void ShowAt(winrt::Microsoft::UI::Xaml::Controls::MenuFlyout& flyout);
};
