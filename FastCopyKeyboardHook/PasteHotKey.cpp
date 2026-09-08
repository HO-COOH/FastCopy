#include "PasteHotKey.h"

PasteHotKey::PasteHotKey(HWND window, int id, UINT modifiers, UINT virtualKey)
    : m_window{ window }, m_id{ id }, m_modifiers{ modifiers }, m_virtualKey{ virtualKey }
{
}

PasteHotKey::~PasteHotKey()
{
    Unregister();
}

bool PasteHotKey::Register()
{
    m_registered = RegisterHotKey(m_window, m_id, m_modifiers, m_virtualKey) != FALSE;
    return m_registered;
}

bool PasteHotKey::Unregister()
{
    if (m_registered)
    {
        UnregisterHotKey(m_window, m_id);
        m_registered = false;
        return true;
    }
    
    return false;
}
