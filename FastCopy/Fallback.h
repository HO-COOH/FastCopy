#pragma once
#include <filesystem>
namespace Fallback
{
	/**
	 * @brief Use `std::filesystem::copy` to copy duplicated name file
	 */
	void CopyAddSuffix(std::filesystem::path const& source, std::filesystem::path&& destination, std::wstring const& suffix, bool isMove);
}
