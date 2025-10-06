#include "DllIconFormatter.h"
#include <Windows.h>
#include <array>
#include "resource.h"
#include <format>
#include "IconProvider.h"

std::wstring_view DllIconFormatter::currentDllPath()
{
	static std::array<wchar_t, MAX_PATH> path = []
	{
		std::array<wchar_t, MAX_PATH> value;
		GetModuleFileName(GetModuleHandle(L"FastCopyShellExtension.dll"), value.data(), std::size(path));
		return value;
	}();
	return path.data();
}

std::wstring DllIconFormatter::GetForRootCommand()
{
	return std::format(L"{},{}", currentDllPath(), -IDI_ICON1);
}

std::wstring DllIconFormatter::GetForSubCommand(CopyOperation op)
{
	return std::format(
		L"{},{}",
		currentDllPath(),
		-IconProvider::GetForCurrentTheme().GetForOperation(op)
	);
}
