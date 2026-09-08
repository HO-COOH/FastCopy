#include "KeyboardHook.h"
#include "KeyboardHookApp.h"

KeyboardHook* KeyboardHook::s_instance = nullptr;

KeyboardHook::KeyboardHook(HINSTANCE instance, KeyboardHookApp* owner)
    : m_owner{ owner },
      m_hook{ SetWindowsHookExW(WH_KEYBOARD_LL, &KeyboardHook::procedure, instance, 0) }
{
    s_instance = this;
}

KeyboardHook::~KeyboardHook()
{
    if (s_instance == this)
    {
        s_instance = nullptr;
    }
}

LRESULT CALLBACK KeyboardHook::procedure(int code, WPARAM wParam, LPARAM lParam)
{
    // Anything but HC_ACTION must be passed on untouched, and lParam only holds
    // a KBDLLHOOKSTRUCT for HC_ACTION.
    if (code != HC_ACTION || !s_instance)
    {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    // A replay is the paste that was already swallowed on its way to Explorer.
    // If this is a replay, do not handle it
    auto const& event = *reinterpret_cast<KBDLLHOOKSTRUCT const*>(lParam);
    if (isReplayInput(event))
    {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    switch (wParam)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (s_instance->m_owner->onKeyDown(event))
                return 1;
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (s_instance->m_owner->onKeyUp(event))
                return 1;
            break;
        default:
            break;
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void KeyboardHook::ReplayPaste()
{
    INPUT input[4]{};
    UINT count{};

    // Ctrl is usually still physically held, and synthesizing a release for a key
    // that is still down would desync the modifier state.
    auto const controlIsDown = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    if (!controlIsDown)
    {
        input[count].type = INPUT_KEYBOARD;
        input[count++].ki.wVk = VK_CONTROL;
    }

    // Both halves, because the key release was swallowed along with the press.
    input[count].type = INPUT_KEYBOARD;
    input[count++].ki.wVk = L'V';
    input[count].type = INPUT_KEYBOARD;
    input[count].ki.wVk = L'V';
    input[count++].ki.dwFlags = KEYEVENTF_KEYUP;

    if (!controlIsDown)
    {
        input[count].type = INPUT_KEYBOARD;
        input[count].ki.wVk = VK_CONTROL;
        input[count++].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    for (UINT index = 0; index < count; ++index)
    {
        input[index].ki.dwExtraInfo = ReplayInputMarker;
    }
    SendInput(count, input, sizeof(INPUT));
}
