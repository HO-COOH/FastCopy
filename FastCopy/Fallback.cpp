#include "pch.h"
#include "Fallback.h"
#include <format>

namespace Fallback
{
	static bool copyFolderAddSuffixImpl(std::filesystem::path const& source, std::filesystem::path const& destination)
	{
		auto const folderName = source.filename().wstring();
		std::error_code ec;
		for (int suffix = 1;; ++suffix)
		{
			auto const fullPath = std::format(LR"({}\{}({}))", destination.wstring(), folderName, suffix);
			if (!std::filesystem::exists(fullPath))
			{

				std::filesystem::copy(source, fullPath, std::filesystem::copy_options::recursive
					|std::filesystem::copy_options::update_existing, ec);
				return bool{ ec };
			}
		}
	}

	static bool copyFileAddSuffixImpl(std::filesystem::path const& source, std::filesystem::path const& destination)
	{
		auto const fileName = source.filename().wstring();
		auto const fileNameWithoutExtension = source.stem().wstring();
		auto const extension = source.extension().wstring();
		std::error_code ec;
		for(int suffix = 1;;++suffix)
		{
			auto fullPath = std::format(LR"({}\{}({}){})", destination.wstring(), fileNameWithoutExtension, suffix, extension);
			if(!std::filesystem::exists(fullPath))
			{
				std::filesystem::copy(source, fullPath);
				return bool{ ec };
			}
		}
	}

	void CopyAddSuffix(std::filesystem::path const& source, std::filesystem::path const& destination, bool isMove)
	{
		bool error = false;
		if (std::filesystem::is_directory(source))
			error = copyFolderAddSuffixImpl(source, destination);
		else
			error = copyFileAddSuffixImpl(source, destination);
		
		if (!error && isMove)
			std::filesystem::remove_all(source);
	}
}