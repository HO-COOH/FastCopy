#pragma once
#include <string_view>
namespace ShellCopy
{
	/**
	 * @brief Returns whether `source` and `destination` are under the same disk parition,
	 * so that we use COM api
	 */
	bool IsInSameDiskPartition(std::wstring_view source, std::wstring_view destination);

	/**
	 * @brief Use COM api to move file
	 */
	bool Move(std::wstring_view source, std::wstring_view destination);
};

