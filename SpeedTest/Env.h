#pragma once
#include <string>
namespace Env
{
	enum class SpecialFolder
	{
		Desktop
	};

	std::wstring const& GetFolderPath(SpecialFolder folder);
}