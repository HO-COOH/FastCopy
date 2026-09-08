#include "pch.h"
#include "KeyboardHookController.h"
#include "../Public/KeyboardHookSettings.h"
#include "../Public/ModulePath.h"

#include <wil/resource.h>
#include <wil/result_macros.h>

#include <filesystem>

bool KeyboardHookController::IsEnabled()
{
    return KeyboardHookSettings::IsEnabled();
}

void KeyboardHookController::SetEnabled(bool enabled)
{
    if (!KeyboardHookSettings::SetEnabled(enabled))
    {
        return;
    }

    enabled ? Start() : Stop();
}

void KeyboardHookController::Start()
{
    if (!IsEnabled())
    {
        return;
    }

    auto const hookPath = std::filesystem::path{ ModulePath() }.parent_path() / L"FastCopyKeyboardHook.exe";
    THROW_HR_IF(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), !std::filesystem::exists(hookPath));

    STARTUPINFOW startupInfo{ sizeof(startupInfo) };
    wil::unique_process_information processInfo;
    THROW_IF_WIN32_BOOL_FALSE(CreateProcessW(
        hookPath.c_str(),
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        hookPath.parent_path().c_str(),
        &startupInfo,
        processInfo.addressof()));
}

void KeyboardHookController::Stop()
{
    auto const window = FindWindowW(KeyboardHookSettings::WindowClassName, nullptr);
    if (window)
    {
        PostMessageW(window, WM_CLOSE, 0, 0);
    }
}
