#include "ClipboardFormat.h"

ClipboardFormat::ClipboardFormat(UINT format)
    : m_handle{ GetClipboardData(format) }
    , m_lock{ m_handle }
{
}

ClipboardFormat::operator bool() const
{
    return m_lock.get() != nullptr;
}

std::byte const* ClipboardFormat::data() const
{
    return static_cast<std::byte const*>(m_lock.get());
}

SIZE_T ClipboardFormat::size() const
{
    return GlobalSize(m_handle);
}
