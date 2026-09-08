#include "DllIconFormatter.h"
#include <Windows.h>
#include <format>
#include "ModulePath.h"
#include "resource.h"
#include "IconProvider.h"

std::wstring DllIconFormatter::GetForRootCommand()
{
	return std::format(L"{},{}", ModulePath(GetModuleHandleW(L"FastCopyShellExtension.dll")), -IDI_ICON1);
}

std::wstring DllIconFormatter::GetForSubCommand(CopyOperation op)
{
	return std::format(
		L"{},{}",
		ModulePath(GetModuleHandleW(L"FastCopyShellExtension.dll")),
		-IconProvider::GetForCurrentTheme().GetForOperation(op)
	);
}
