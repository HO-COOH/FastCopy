#include "pch.h"
#include "DebugLogger.h"

void DebugLogger::Log(std::wstring_view arg)
{
    OutputDebugStringW(arg.data());
    OutputDebugStringW(L"\n");
}

DebugLogger& DebugLogger::GetInstance()
{
    static DebugLogger logger;
    return logger;
}
