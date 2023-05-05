#include "ShellItemArray.h"
#include "ShellItem.h"
#include <ShObjIdl_core.h>

ShellItemArray::Iterator ShellItemArray::begin()
{
    return { m_ptr };
}

ShellItemArray::Iterator ShellItemArray::end()
{
    return {nullptr, size() };
}

ShellItem ShellItemArray::front()
{
    return (*this)[0];
}

ShellItem ShellItemArray::back()
{
    return (*this)[size() - 1];
}

ShellItem ShellItemArray::operator[](size_t index)
{
    IShellItem* item;
    m_ptr->GetItemAt(index, &item);
    return ShellItem{ item };
}

size_t ShellItemArray::size()
{
    if (!m_ptr)
        return 0;

    DWORD count;
    m_ptr->GetCount(&count);
    return count;
}

ShellItem ShellItemArray::Iterator::operator*()
{
    return ShellItemArray{ m_ptr } [m_index] ;
}

ShellItemArray::Iterator& ShellItemArray::Iterator::operator++()
{
    ++m_index;
    return *this;
}

bool ShellItemArray::Iterator::operator==(Iterator const& rhs) const
{
    return rhs.m_ptr == nullptr ? m_index == rhs.m_index :
        m_ptr == rhs.m_ptr && m_index == rhs.m_index;
}
