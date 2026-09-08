#include "KeyboardHookApp.h"
#include "ClipboardFileTransfer.h"
#include "ExplorerWindow.h"
#include "COMInitializeHelper.h"
#include <objbase.h>
#include <winrt/base.h>


KeyboardHookApp::KeyboardHookApp(HINSTANCE instance)
    : m_window{ instance, this },
      m_pasteHotKey{ m_window.Handle(), pasteHotKeyId, MOD_CONTROL | MOD_NOREPEAT, L'V' },
      m_keyboardHook{ instance, this }
{
}

void KeyboardHookApp::ShowError(char const* message)
{
    winrt::hstring wide = message ? winrt::to_hstring(message) : L"Unknown error";
    MessageBoxW(nullptr, wide.c_str(), L"RoboCopyEx", MB_OK | MB_ICONERROR);
}

int KeyboardHookApp::Run()
{
    COMInitializeHelper comHelper;

    if (!m_pasteHotKey.Register() && !m_keyboardHook.Installed())
    {
        ShowError(
            "Ctrl+V could not be intercepted: neither the hot key nor the keyboard hook could be installed."
        );
        return 1;
    }

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return 0;
}

LRESULT KeyboardHookApp::onHotKey(int hotKeyId)
{
    if (hotKeyId == pasteHotKeyId && !m_pasteKeyGesture)
    {
        requestPaste(GetForegroundWindow());
    }
    return 0;
}

LRESULT KeyboardHookApp::onPasteRequested(HWND expectedExplorerWindow)
{
    try
    {
        handlePaste(expectedExplorerWindow);
    }
    catch (wil::ResultException const& e)
    {
        ShowError(e.what());
    }
    catch (std::exception const& e)
    {
        ShowError(e.what());
    }
    return 0;
}

LRESULT KeyboardHookApp::onTimer(UINT_PTR timerId)
{
    if (timerId == pasteHotKeyRestoreTimerId)
    {
        KillTimer(m_window.Handle(), pasteHotKeyRestoreTimerId);
        m_pasteHotKey.Register();
    }
    return 0;
}

void KeyboardHookApp::updateModifierState(DWORD virtualKey, bool pressed)
{
    switch (virtualKey)
    {
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_CONTROL:
            m_controlDown = pressed;
            return;
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:
            m_shiftDown = pressed;
            return;
        case VK_LMENU:
        case VK_RMENU:
        case VK_MENU:
            m_altDown = pressed;
            return;
        case VK_LWIN:
        case VK_RWIN:
            m_winDown = pressed;
            return;
        default:
            return;
    }
}

bool KeyboardHookApp::onKeyDown(KBDLLHOOKSTRUCT const& event)
{
    updateModifierState(event.vkCode, true);
    if (event.vkCode != L'V')
    {
        return false;
    }

    // Begin a paste gesture: Ctrl+V pressed in an Explorer window.
    auto const modifiersMatch = m_controlDown && !m_shiftDown && !m_altDown && !m_winDown;
    if (!modifiersMatch || !IsExplorerWindow(GetForegroundWindow()))
    {
        return false;
    }

    // Auto-repeat while the key is held reaches here again; suppress it without
    // queueing a second paste.
    if (!m_interceptedPasteKey)
    {
        m_interceptedPasteKey = true;
        m_pasteKeyGesture = true;
        requestPaste(GetForegroundWindow());
    }
    return true;
}

bool KeyboardHookApp::onKeyUp(KBDLLHOOKSTRUCT const& event)
{
    updateModifierState(event.vkCode, false);
    if (event.vkCode != L'V')
    {
        return false;
    }

    // End an intercepted paste gesture on key-up.
    if (!m_interceptedPasteKey)
    {
        return false;
    }

    m_interceptedPasteKey = false;
    m_pasteKeyGesture = false;
    return true;
}

void KeyboardHookApp::handlePaste(HWND expectedExplorerWindow)
{
    m_pasteRequestQueued = false;

    // Each step gates the next: the clipboard is not read without a folder to paste
    // into, and nothing is launched until both are in hand.
    if (auto const destination = GetExplorerFolder(expectedExplorerWindow))
    {
        auto transfer = ClipboardFileTransfer::Read();
        if (transfer && transfer->Paste(*destination))
        {
            if (transfer->move)
            {
                clearMoveClipboard();
            }
            return;
        }
    }

	// When failed, replays ctrl+v when the foreground window is indeed the same one 
    // that the paste was requested
    if (GetForegroundWindow() == expectedExplorerWindow)
    {
        replayPaste();
    }
}

void KeyboardHookApp::requestPaste(HWND foregroundWindow)
{
    if (m_pasteRequestQueued)
    {
        return;
    }

    m_pasteRequestQueued = true;
    if (!PostMessageW(m_window.Handle(), PasteRequest, reinterpret_cast<WPARAM>(foregroundWindow), 0))
    {
        m_pasteRequestQueued = false;
    }
}

void KeyboardHookApp::replayPaste()
{
    // The hot key outranks normal keyboard routing, so it would capture the replayed
    // Ctrl+V before Explorer ever saw it. SendInput only queues the events, hence the
    // timer rather than registering again right away.
    if (m_pasteHotKey.Unregister())
        SetTimer(m_window.Handle(), pasteHotKeyRestoreTimerId, 250, nullptr);

    KeyboardHook::ReplayPaste();
}

void KeyboardHookApp::clearMoveClipboard()
{
    if (OpenClipboard(m_window.Handle()))
    {
        EmptyClipboard();
        CloseClipboard();
    }
}
