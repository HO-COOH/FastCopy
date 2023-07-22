#include "pch.h"
#include "Fallback.h"
#include <format>
#include <ShlObj_core.h>
#include "PathUtils.h"

namespace Fallback
{
	/**
	 * @brief Wrapper for creating a unique name based on the duplicate destination full path
	 * 
	 * @param destinationFullPath The "intended" full destinationm 
	 */
	static auto MakeUniqueName(std::wstring_view destinationFullPath)
	{
		std::wstring buffer(destinationFullPath.size() + 10, {});
		PathYetAnotherMakeUniqueName(buffer.data(), destinationFullPath.data(), nullptr, nullptr);
		return buffer;
	}

	/**
	 * @brief Copy with unique name copying impl
	 * 
	 * @param source Full path to the source file
	 * @param destination Full path to the destination folder
	 * @retval true If error **happens**
	 * @retval false If no error
	 */
	static bool copyFileAddSuffixImpl(std::filesystem::path const& source, std::filesystem::path const& destination)
	{
		auto const fileName = source.filename().wstring();
		std::error_code ec;
		auto fullPath = MakeUniqueName(std::format(LR"({}\{})", ToBackslash(destination.wstring()), fileName));
		assert(!std::filesystem::exists(fullPath));
		std::filesystem::copy(source, fullPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing, ec);
		return bool{ ec };
	}

	void CopyAddSuffix(std::filesystem::path const& source, std::filesystem::path const& destination, bool isMove)
	{
		if (!copyFileAddSuffixImpl(source, destination) && isMove)
		{
			//there is error OR isMove
			std::filesystem::remove_all(source);
		}
	}
}