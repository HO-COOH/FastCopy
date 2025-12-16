#pragma once
#include <string>

namespace std::filesystem
{
	class path;
}

namespace Utils
{
	//return a localized " - Copy" suffix
	std::wstring GetDefaultRenameSuffix();

	void AddDestinationSuffixIfNeeded(
		bool isDirectory,
		std::filesystem::path const& source,
		std::filesystem::path& destination
	);
}