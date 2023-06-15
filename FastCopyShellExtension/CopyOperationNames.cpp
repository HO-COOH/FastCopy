#include "CopyOperationNames.h"
#include <Windows.h>
#include <algorithm>

static std::wstring getDefaultLanguage()
{
    ULONG bytes{};
    ULONG numLanguages{};
    GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, nullptr, &bytes);
    std::wstring buffer(bytes, {});
    GetUserPreferredUILanguages(MUI_LANGUAGE_NAME, &numLanguages, buffer.data(), &bytes);
    return std::wstring{ buffer.data() };
}

CopyOperationNames& CopyOperationNames::GetInstance()
{
    static CopyOperationNames s_instance = []
    {
        auto defaultLang = getDefaultLanguage();
        std::transform(defaultLang.begin(), defaultLang.end(), defaultLang.begin(), [](wchar_t c) { return towlower(c); });
        if (defaultLang == L"en-us")
        {
            return CopyOperationNames
            {
                .Copy = L"Copy",
                .Move = L"Move",
                .Paste = L"Paste",
                .Delete = L"Delete"
            };
        }
        else if (defaultLang == L"zh-cn")
        {
            return CopyOperationNames
            {
                .Copy = L"复制",
                .Move = L"剪切",
                .Paste = L"粘贴",
                .Delete = L"删除"
            };
        }
        //Add more languages here
    }();
    return s_instance;
}
