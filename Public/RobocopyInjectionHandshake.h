#pragma once
#include <Windows.h>
#include <string_view>

namespace RobocopyInjectionHandshake
{
    constexpr inline std::string_view ReadyLine = "[FastCopy:InjectOK]\n";
    constexpr inline std::string_view ReadyMarker = ReadyLine.substr(0, ReadyLine.size() - 1);

    /*
        Uses the Windows customer NTSTATUS layout so exit codes never collide with
        robocopy exit codes (0..16) or OS NTSTATUS crash/loader codes (0xC000xxxx):
          Bits 31..30 = 11 (Severity: Error -> 0xC0000000)
          Bit  29     =  1 (Customer-defined flag -> 0x20000000, combined = 0xE0000000)
          Bits 15..0       (Step-specific error code)
        See: https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/87fba13e-bf06-450e-83b1-9241dc81e781
    */
    enum class InjectionStatus : DWORD
    {
        ForceEnglishFailed = 0xE0000001u,
        HookWfopenFailed = 0xE0000002u,
        SetConsoleUtf8Failed = 0xE0000003u,
    };
}
