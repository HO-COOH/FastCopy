#pragma once
#include <string>
/**
 * This is made into a class because we maybe want to localize it in the future
 */
struct CopyOperationNames
{
    std::wstring Copy = L"Copy";
    std::wstring Move = L"Move";
    std::wstring Paste = L"Paste";
    std::wstring Delete = L"Delete";

    static CopyOperationNames& GetInstance();
};
