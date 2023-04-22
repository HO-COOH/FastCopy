#include "IconProvider.h"
#include <winrt/windows.ui.viewmanagement.h>


//Copied from https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/apply-windows-themes
inline static bool isColorLight(winrt::Windows::UI::Color clr)
{
    return (((5 * clr.G) + (2 * clr.R) + clr.B) > (8 * 128));
}

IconProvider const& IconProvider::GetForCurrentTheme()
{
    static winrt::Windows::UI::ViewManagement::UISettings settings;
    static IconProvider provider
    {
        isColorLight(winrt::Windows::UI::ViewManagement::UISettings{}.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Foreground)) ? 
        ThemeType::Light : ThemeType::Dark
    };
    return provider;
}

int IconProvider::GetForOperation(CopyOperation op) const
{
    if (m_themeType == ThemeType::Light)
    {
        switch (op)
        {
            case CopyOperation::Copy:   return IDI_ICON2;
            case CopyOperation::Move:   return IDI_ICON6;
            case CopyOperation::Paste:  return IDI_ICON10;
            case CopyOperation::Delete: return IDI_ICON14;
        }
    }
    else
    {
        switch (op)
        {
            case CopyOperation::Copy:   return IDI_ICON4;
            case CopyOperation::Move:   return IDI_ICON8;
            case CopyOperation::Paste:  return IDI_ICON12;
            case CopyOperation::Delete: return IDI_ICON15;
        }
    }
    return 0;
}
