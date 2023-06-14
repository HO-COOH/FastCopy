#pragma once
#include <string_view>
/**
 * This is made into a class because we maybe want to localize it in the future
 */
struct CopyOperationNames
{
    std::wstring_view Copy = L"Copy";
    std::wstring_view Move = L"Move";
    std::wstring_view Paste = L"Paste";
    std::wstring_view Delete = L"Delete";

    static CopyOperationNames& GetInstance();
};
