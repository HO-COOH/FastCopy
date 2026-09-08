#include "ModulePath.h"

#include <array>

std::wstring_view ModulePath(HMODULE module)
{
    static std::array<wchar_t, MAX_PATH> buffer{};
    auto const length = GetModuleFileNameW(module, buffer.data(), static_cast<DWORD>(buffer.size()));
    return length > 0 && length < buffer.size() ? std::wstring_view{ buffer.data(), length } : std::wstring_view{};
}
