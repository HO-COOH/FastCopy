#pragma once
#include <wil/com.h>
#include <ExDisp.h>
#include "WebBrowser2.h"
#include <optional>

class ShellWindows
{
	wil::com_ptr<IShellWindows> m_ptr = wil::CoCreateInstance<IShellWindows>(CLSID_ShellWindows, CLSCTX_ALL);
public:
	[[nodiscard]] long Count();
	[[nodiscard]] WebBrowser2 Item(VARIANT index);

	// Get information about all currently open explorer windows.
	[[nodiscard]] static std::optional<WebBrowser2> GetForegroundExplorer();
};
