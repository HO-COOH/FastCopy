#pragma once
#include <wil/com.h>
#include <wtypes.h>

struct IShellItem2;

class ShellItem2
{
	wil::com_ptr<IShellItem2> m_ptr;
public:
	ShellItem2(IShellItem2* ptr = nullptr);
	ShellItem2(wchar_t const* path);
	~ShellItem2();

	wil::unique_cotaskmem_string GetString(PROPERTYKEY key);

	operator bool() const;
};