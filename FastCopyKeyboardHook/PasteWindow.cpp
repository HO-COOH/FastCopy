#include "PasteWindow.h"
#include "KeyboardHookApp.h"

#include <wil/result_macros.h>

PasteWindow::PasteWindow(HINSTANCE instance, KeyboardHookApp* owner) : m_owner{ owner }
{
    WNDCLASSW windowClass
    {
        .lpfnWndProc = &PasteWindow::windowProc,
        .hInstance = instance,
        .lpszClassName = KeyboardHookSettings::WindowClassName,
    };
    THROW_LAST_ERROR_IF(!RegisterClassW(&windowClass));

    m_window.reset(CreateWindowExW(
        0,
        KeyboardHookSettings::WindowClassName,
        nullptr,
        0,
        0,
        0,
        0,
        0,
        HWND_MESSAGE,
        nullptr,
        instance,
        this));
    THROW_LAST_ERROR_IF(!m_window);
}

LRESULT CALLBACK PasteWindow::windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_NCCREATE:
        {
            auto const create = reinterpret_cast<CREATESTRUCTW const*>(lParam);
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
            return TRUE;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            DestroyWindow(window);
            return 0;
        case WM_HOTKEY:
            return owner(window)->onHotKey(static_cast<int>(wParam));
        case KeyboardHookApp::PasteRequest:
            return owner(window)->onPasteRequested(reinterpret_cast<HWND>(wParam));
        case WM_TIMER:
            return owner(window)->onTimer(static_cast<UINT_PTR>(wParam));
        default:
            return DefWindowProcW(window, message, wParam, lParam);
    }
}

KeyboardHookApp* PasteWindow::owner(HWND window)
{
    // Set by WM_NCCREATE, which precedes every message handled above.
    auto const self = reinterpret_cast<PasteWindow*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    return self->m_owner;
}
