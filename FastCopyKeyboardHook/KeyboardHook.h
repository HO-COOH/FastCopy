#pragma once
#include <wil/resource.h>
#include <Windows.h>

class KeyboardHookApp;

// Installs a WH_KEYBOARD_LL hook and removes it on destruction (RAII).
// It owns the single switch over the key events: hook plumbing is handled here,
// key presses and releases call straight into the owner's handler for each.
class KeyboardHook
{
public:
    KeyboardHook(HINSTANCE instance, KeyboardHookApp* owner);
    ~KeyboardHook();

    KeyboardHook(KeyboardHook const&) = delete;
    KeyboardHook& operator=(KeyboardHook const&) = delete;

    bool Installed() const { return m_hook != nullptr; }

    // Synthesizes a Ctrl+V, tagged so that the hook recognizes it as its own and
    // passes it through untouched. Static because the replay is still needed when
    // the hook could not be installed.
    static void ReplayPaste();

private:
    static LRESULT CALLBACK procedure(int code, WPARAM wParam, LPARAM lParam);
    
    static constexpr ULONG_PTR ReplayInputMarker = 0x52435856;
    
    constexpr static bool isReplayInput(KBDLLHOOKSTRUCT const& event)
    {
        // dwExtraInfo is not a trust boundary, so the injected flag is required as well:
        // a real keystroke can then never be mistaken for a replay.
        return (event.flags & LLKHF_INJECTED) != 0 && event.dwExtraInfo == ReplayInputMarker;
    }

    static KeyboardHook* s_instance;

    KeyboardHookApp* m_owner{};
    wil::unique_hhook m_hook;
};
