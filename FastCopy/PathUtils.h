#pragma once
#include <string_view>
#include <string>
#include <ShObjIdl_core.h>

/**
 * @brief Convert forward slash to back slash, returning a new string
 */
inline std::wstring ToBackslash(std::wstring_view source)
{
    std::wstring ret(source.size(), {});
    std::ranges::transform(source, ret.begin(), [](wchar_t c) {return c == L'/' ? L'\\' : c; });
    return ret;
}

/**
 * @brief Returns a `IShellItem`(default) or `IShellItem2` from `path`
 * @warning Throws if operation failed
 */
template<typename ShellItemInterface = IShellItem>
    requires std::is_same_v<ShellItemInterface, IShellItem> || std::is_same_v<ShellItemInterface, IShellItem2>
auto CreateItemFromParsingName(wchar_t const* path)
{
    winrt::com_ptr<ShellItemInterface> item;
    winrt::check_hresult(SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(item.put())));
    return item;
}
