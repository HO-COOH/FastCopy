#include "ShellItem.h"
#include <ShObjIdl_core.h>

ShellItem::ShellItem(std::wstring_view path)
{
    THROW_IF_FAILED(SHCreateItemFromParsingName(path.data(), nullptr, IID_PPV_ARGS(m_ptr.put())));
}

wchar_t* ShellItem::GetDisplayName() const
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
