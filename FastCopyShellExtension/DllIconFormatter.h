#pragma once
#include <string>
#include "CopyOperation.h"

class DllIconFormatter
{
	static std::wstring_view currentDllPath();
public:
	static std::wstring GetForRootCommand();
	static std::wstring GetForSubCommand(CopyOperation op);
};
