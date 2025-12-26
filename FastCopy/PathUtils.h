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
 * @brief Returns a smart pointer to `IShellItem` or `IShellItem2` from `path`
 * @tparam SmartPtr A smart pointer template (e.g., winrt::com_ptr, wil::com_ptr)
 * @tparam ShellItemInterface The shell item interface type (IShellItem or IShellItem2)
 * @warning Throws if operation failed
 */
template<template<typename> class SmartPtr = winrt::com_ptr, typename ShellItemInterface = IShellItem>
    requires std::is_same_v<ShellItemInterface, IShellItem> || std::is_same_v<ShellItemInterface, IShellItem2>
SmartPtr<ShellItemInterface> CreateItemFromParsingName(wchar_t const* path)
{
    SmartPtr<ShellItemInterface> item;
    winrt::check_hresult(SHCreateItemFromParsingName(path, NULL, IID_PPV_ARGS(item.put())));
    return item;
}
