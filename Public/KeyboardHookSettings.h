#pragma once

#include <Windows.h>
#include <wil/registry.h>

namespace KeyboardHookSettings
{
    constexpr wchar_t RegistryPath[] = L"Software\\RoboCopyEx";
    constexpr wchar_t EnabledValueName[] = L"KeyboardHookEnabled";
    constexpr wchar_t WindowClassName[] = L"RoboCopyEx.KeyboardHook.Window";
    constexpr wchar_t SingletonName[] = L"Local\\RoboCopyEx.KeyboardHook.Singleton";

    inline bool IsEnabled()
    {
        auto const value = wil::reg::try_get_value_dword(HKEY_CURRENT_USER, RegistryPath, EnabledValueName);
        // A missing value (first run) means enabled.
        return !value || *value != 0;
    }

    inline bool SetEnabled(bool enabled)
    {
        auto const key = wil::reg::create_unique_key(HKEY_CURRENT_USER, RegistryPath, wil::reg::key_access::readwrite);
        return SUCCEEDED(wil::reg::set_value_dword_nothrow(key.get(), EnabledValueName, enabled ? 1 : 0));
    }
}
