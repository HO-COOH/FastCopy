#pragma once
#include <string>
namespace Env
{
	enum class SpecialFolder
	{
		Desktop,
		System32,
		Windows
	};

	std::wstring const& GetFolderPath(SpecialFolder folder);

	/**
	 * Get available drive for copy destination.
	 * @param subFolder The name of subfolder that will be created under an available drive
	 * @param bytes The size that requires the disk to have for test
	 */
	std::wstring const& GetTestDestinationPath(std::wstring_view subFolder = L"", size_t bytes = 1024 * 1024 * 1024);

	/**
	 * Generate a random folder name for holding test files
	 */
	std::wstring const& GetRandomName();

	//Print to console when it is not running TUI
	void Puts(char const* const arg);
}