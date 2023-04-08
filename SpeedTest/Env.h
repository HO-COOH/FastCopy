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
	 */
	std::wstring const& GetTestDestinationPath(std::wstring_view subFolder = L"", size_t bytes = 1024 * 1024 * 1024);

	/**
	 * Generate a random folder name for holding test files
	 */
	std::wstring const& GetRandomName();
}