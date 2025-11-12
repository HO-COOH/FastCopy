#include "CopyOperationNames.h"
#include <Windows.h>
#include <algorithm>
#include <string>
#include <PackageConfig.h>

CopyOperationNames& CopyOperationNames::GetInstance()
{
    static CopyOperationNames s_instance = []
    {
        auto defaultLang = PackageConfig::GetDefaultLanguage();
        std::transform(defaultLang.begin(), defaultLang.end(), defaultLang.begin(), [](wchar_t c) { return towlower(c); });
        if (defaultLang == L"zh-cn")
        {
            return CopyOperationNames
            {
                .Copy = L"复制",
                .Move = L"剪切",
                .Paste = L"粘贴",
                .Delete = L"删除"
            };
        }
        else if (defaultLang == L"de-de") 
        {
            return CopyOperationNames
            {
                .Copy = L"Kopieren",
                .Move = L"Verschieben",
                .Paste = L"Einfügen",
                .Delete = L"Löschen"
            };
        }
        else if (defaultLang == L"es-es") 
        {
            return CopyOperationNames
            {
                .Copy = L"Copiar",
                .Move = L"Mover",
                .Paste = L"Pegar",
                .Delete = L"Borrar"
            };
        }
        else
        {
            return CopyOperationNames
            {
                .Copy = L"Copy",
                .Move = L"Move",
                .Paste = L"Paste",
                .Delete = L"Delete"
            };
        }
        //Add more languages here
    }();
    return s_instance;
}
