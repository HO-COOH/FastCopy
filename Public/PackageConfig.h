#pragma once
#include <string>
#include <Windows.h>

namespace PackageConfig
{
	constexpr static auto PackageName = L"RoboCopyEx";

    inline std::wstring GetDefaultLanguage()
    {
        ULONG bytes{};
        ULONG numLanguages{};
        GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, nullptr, &bytes);
        std::wstring buffer(bytes, {});
        GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, buffer.data(), &bytes);
        return std::wstring{ buffer.data() };
    }
}
