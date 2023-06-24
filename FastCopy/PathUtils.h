#pragma once
#include <string_view>
#include <string>

/**
 * @brief Convert forward slash to back slash, returning a new string
 */
inline std::wstring ToBackslash(std::wstring_view source)
{
    std::wstring ret(source.size(), {});
    std::transform(source.begin(), source.end(), ret.begin(), [](wchar_t c) {return c == L'/' ? L'\\' : c; });
    return ret;
}
