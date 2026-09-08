#include "FastCopyLauncher.h"
#include <Windows.h>
#include <shellapi.h>
#include <algorithm>
#include <format>
#include <string>

bool LaunchFastCopy(std::wstring_view destination, std::wstring_view recordFile)
{
    auto uri = std::format(LR"(fastcopy://"{}"|"{}")", destination, recordFile);
    std::ranges::replace(uri, L'\\', L'/');
#if (defined _DEBUG) || (defined DEBUG)
    OutputDebugStringW(uri.c_str());
#endif

    // Both callers are windowless or background: the hook has no foreground window worth
    // the name, and the shell extension runs inside Explorer. Without this the main
    // window opens behind the window the user was looking at.
    AllowSetForegroundWindow(ASFW_ANY);
    auto const result = reinterpret_cast<INT_PTR>(ShellExecuteW(
        nullptr,
        L"open",
        uri.c_str(),
        nullptr,
        nullptr,
        SW_SHOWNORMAL));
    return result > 32;
}
