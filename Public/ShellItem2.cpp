#include "pch.h"
#include "ShellItem2.h"
#include <ShlObj_core.h>
#include "PathUtils.h"

ShellItem2::ShellItem2(wchar_t const* path) : m_ptr{ CreateItemFromParsingName<wil::com_ptr, IShellItem2>(path) }
{
}

wil::unique_cotaskmem_string ShellItem2::GetString(PROPERTYKEY key)
{
	wil::unique_cotaskmem_string pValue;
	THROW_IF_FAILED(m_ptr->GetString(key, &pValue));
	return pValue;
}

ShellItem2::operator bool() const
{
	return m_ptr != nullptr;
}