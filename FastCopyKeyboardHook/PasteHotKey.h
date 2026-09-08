#pragma once
#include <Windows.h>

// Registers a hot key on a window and unregisters it on destruction (RAII).
// The hot key can be temporarily unregistered (while a native paste is being
// replayed) and registered again afterwards.
class PasteHotKey
{
public:
    PasteHotKey(HWND window, int id, UINT modifiers, UINT virtualKey);
    ~PasteHotKey();

    PasteHotKey(PasteHotKey const&) = delete;
    PasteHotKey& operator=(PasteHotKey const&) = delete;

    bool Register();
    bool Unregister();
private:
    HWND m_window{};
    int m_id{};
    UINT m_modifiers{};
    UINT m_virtualKey{};
    bool m_registered{};
};
