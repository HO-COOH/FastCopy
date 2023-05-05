#pragma once

struct IShellItem;
class ShellItem
{
	IShellItem* m_ptr;
public:
	ShellItem(IShellItem* ptr = nullptr) :m_ptr{ ptr } {}

	wchar_t* GetDisplayName();
	ShellItem GetParent();

	IShellItem* GetPtr() const { return m_ptr; }
};

