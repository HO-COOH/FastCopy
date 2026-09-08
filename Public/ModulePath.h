#pragma once
#include <Windows.h>
#include <string_view>

// The full path of the given module, or of the current process's main module
// when `module` is nullptr. Empty when the path does not fit the buffer.
[[nodiscard]] std::wstring_view ModulePath(HMODULE module = nullptr);
