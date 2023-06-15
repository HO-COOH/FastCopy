#pragma once

#include <winrt/Windows.Foundation.h>
#include <string_view>

winrt::hstring GetStringResource(std::wstring_view ket);

winrt::hstring ToCapital(winrt::hstring word);