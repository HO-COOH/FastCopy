#pragma once
#include <ole2.h>
#include <wil/resource.h>
#include <cstddef>

class ClipboardFormat
{
    HGLOBAL m_handle;
    wil::unique_hglobal_locked m_lock;
public:
    ClipboardFormat(UINT format);

    operator bool() const;
    std::byte const* data() const;
    SIZE_T size() const;
};
