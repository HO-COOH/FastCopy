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

	void AddDestinationSuffixIfNeededForFile(std::filesystem::path const& source, std::filesystem::path& destination, std::wstring const& suffix);

	void AddDestinationSuffixIfNeededForDirectory(std::filesystem::path const& source, std::filesystem::path& destination, std::wstring const& suffix);

	bool IsRenameSuffixValid(winrt::hstring const& suffix);
}