#pragma once
#include <filesystem>
namespace Fallback
{
	void CopyAddSuffix(std::filesystem::path const& source, std::filesystem::path const& destination, bool isMove);
}
