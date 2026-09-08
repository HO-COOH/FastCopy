#pragma once
#include <string>
#include "CopyOperation.h"

class DllIconFormatter
{
public:
	static std::wstring GetForRootCommand();
	static std::wstring GetForSubCommand(CopyOperation op);
};
