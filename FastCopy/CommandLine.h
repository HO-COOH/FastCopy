#pragma once
#include <vector>
#include <string_view>
#include "../FastCopyShellExtension/CopyOperation.h"

std::vector<std::wstring_view> const& GetCommandArgs();

class Command
{
public:
	static Command& Get();
	CopyOperation GetOperation();
	std::wstring RecordFile();
};
