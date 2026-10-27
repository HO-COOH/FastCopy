#pragma once
#include <string_view>
#include <string>
#include <filesystem>
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
 * @brief Convert a UTF-8 string (eg. a path in robocopy's output) to a path, invalid bytes become U+FFFD
 * @note Unlike `std::filesystem::path{ std::string }` (CRT locale) or `{ std::u8string_view }`
 * this never throws on bytes that do not decode
 */
inline std::filesystem::path Utf8ToPath(std::string_view utf8)
{
    auto const length = static_cast<int>(utf8.size());
    std::wstring wide(MultiByteToWideChar(CP_UTF8, 0, utf8.data(), length, nullptr, 0), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.data(), length, wide.data(), static_cast<int>(wide.size()));
    return std::filesystem::path{ std::move(wide) };
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
