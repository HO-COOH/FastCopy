#pragma once
#include <filesystem>
namespace Fallback
{
	/**
	 * @brief Use `std::filesystem::copy` to copy duplicated name file
	 */
	void CopyAddSuffix(std::filesystem::path const& source, std::filesystem::path const& destination, bool isMove);
}
