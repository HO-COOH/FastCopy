#pragma once
#include <wil/com.h>
#include <string_view>

struct IShellItem;

class ShellItem
{
	wil::com_ptr<IShellItem> m_ptr;
public:
	ShellItem(IShellItem* ptr = nullptr) :m_ptr{ ptr } {}
	ShellItem(std::wstring_view path);

	wchar_t* GetDisplayName();
	ShellItem GetParent();

	IShellItem* GetPtr() const { return m_ptr.get(); }
};

