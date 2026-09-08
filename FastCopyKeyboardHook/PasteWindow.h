#pragma once
#include "../Public/KeyboardHookSettings.h"

#include <Windows.h>
#include <wil/resource.h>

class KeyboardHookApp;

// The message-only window (HWND_MESSAGE) that receives hot-key and paste messages
// and also manages app lifetime
class PasteWindow
{
public:
    PasteWindow(HINSTANCE instance, KeyboardHookApp* owner);

    PasteWindow(PasteWindow const&) = delete;
    PasteWindow& operator=(PasteWindow const&) = delete;

    HWND Handle() const { return m_window.get(); }

private:
    static LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    static KeyboardHookApp* owner(HWND window);

    // Declared before m_window so the owner is set before the window can
    // dispatch a message, and still set while the window is being destroyed.
    KeyboardHookApp* m_owner{};
    wil::unique_hwnd m_window{};
};
