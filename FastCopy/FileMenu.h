#pragma once

#include "FileMenu.g.h"
#include <ShObjIdl_core.h>

namespace winrt::FastCopy::implementation
{
    struct FileInfoViewModel;

    struct MenuData
    {
        unsigned int menuId;
        std::wstring verb;
    };


    struct FileMenu : FileMenuT<FileMenu>
    {
        FileMenu() = default;

        winrt::FastCopy::FileInfoViewModel File();
        void File(winrt::FastCopy::FileInfoViewModel const& value);

    private:
        winrt::FastCopy::FileInfoViewModel m_fileInfo{ nullptr };
        winrt::FastCopy::implementation::FileInfoViewModel* m_fileInfoImpl{ nullptr };

        std::vector<MenuData> m_menuData;
        winrt::com_ptr<IContextMenu> m_menu;

        std::vector<winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItemBase> menuFlyoutFromHMenu(
            HMENU menu
        );

        winrt::Microsoft::UI::Xaml::Controls::MenuFlyoutItem makeMenuFlyout(MENUITEMINFO const& menuItemInfo);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct FileMenu : FileMenuT<FileMenu, implementation::FileMenu>
    {
    };
}
