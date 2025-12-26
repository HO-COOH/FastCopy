#pragma once
#include <wil/com.h>

struct IShellItem;

class ShellItem
{
	wil::com_ptr<IShellItem> m_ptr;
public:
	ShellItem(IShellItem* ptr = nullptr) :m_ptr{ ptr } {}
	ShellItem(wchar_t const* path);

	wchar_t* GetDisplayName() const;
	ShellItem GetParent();

	IShellItem* Get() const { return m_ptr.get(); }
};

