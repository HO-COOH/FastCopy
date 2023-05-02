#include "ShellItem.h"
#include <ShObjIdl_core.h>

wchar_t* ShellItem::GetDisplayName()
{
    wchar_t* ret;
    m_ptr->GetDisplayName(SIGDN_FILESYSPATH, &ret);
    return ret;
}

ShellItem ShellItem::GetParent()
{
    IShellItem* parent;
    m_ptr->GetParent(&parent);
    return ShellItem{ parent };
}
