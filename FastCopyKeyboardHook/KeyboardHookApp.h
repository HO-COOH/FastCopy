#pragma once
#include "PasteHotKey.h"
#include "KeyboardHook.h"
#include "PasteWindow.h"

#include <Windows.h>

// Owns the paste integration: the hidden window, the Ctrl+V hot key and the
// low-level keyboard hook, plus the paste handling logic.
class KeyboardHookApp
{
public:
    KeyboardHookApp(HINSTANCE instance);
    int Run();
    static void ShowError(char const* message);

private:
    static constexpr auto PasteRequest = WM_APP + 1;
    static constexpr int pasteHotKeyId = 1;
    static constexpr UINT_PTR pasteHotKeyRestoreTimerId = 1;

    // One per window message PasteWindow dispatches; the message constants are
    // owned here so PasteWindow's switch can label its cases with them.
    LRESULT onHotKey(int hotKeyId);
    LRESULT onPasteRequested(HWND expectedExplorerWindow);
    LRESULT onTimer(UINT_PTR timerId);

    // One per key event KeyboardHook dispatches. They return true when the key is
    // swallowed, so the hook procedure owns the CallNextHookEx fallback. Replayed
    // input is filtered out by the hook and never reaches them.
    bool onKeyDown(KBDLLHOOKSTRUCT const& event);
    bool onKeyUp(KBDLLHOOKSTRUCT const& event);

    void updateModifierState(DWORD virtualKey, bool pressed);
    void handlePaste(HWND expectedExplorerWindow);
    void requestPaste(HWND foregroundWindow);
    void replayPaste();
    void clearMoveClipboard();

    PasteWindow m_window;
    PasteHotKey m_pasteHotKey;
    KeyboardHook m_keyboardHook;
    bool m_interceptedPasteKey{};
    bool m_pasteRequestQueued{};
    bool m_pasteKeyGesture{};
    bool m_controlDown{};
    bool m_shiftDown{};
    bool m_altDown{};
    bool m_winDown{};

    friend class PasteWindow;
    friend class KeyboardHook;
};
