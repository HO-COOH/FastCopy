#include "pch.h"
#include "RenameUtils.h"
#include <chrono>
#include <wil/resource.h>
#include <shellapi.h>
#include <filesystem>
#include "Settings.h"
#include <boost/algorithm/string.hpp>
#include "RenameUtils.TempFileAutoDeleter.h"

/**
 * @param original Test.txt
 * @param generated Test - Copy.txt
 */
static std::wstring extractCopyString(const std::wstring& original, std::wstring_view generated) 
{
	auto const dotPos = original.rfind(L'.');
	auto const suffixLength = generated.length() - original.length();
	return std::wstring(generated.substr(dotPos, suffixLength));
}


namespace Utils
{
	std::wstring GetDefaultRenameSuffix()
	{
		//create a test.txt file
		wchar_t tempDir[MAX_PATH]{};
		GetTempPath(MAX_PATH, tempDir);

		auto baseName = std::to_wstring(std::chrono::system_clock::now().time_since_epoch().count());
		auto fileName = std::format(L"{}.txt", baseName);
		auto path = std::format(L"{}{}", tempDir, fileName);

		TempFileAutoDeleter tempFile{ path, true };

		//Call SHFileOperationW
		{
			path.push_back(L'\0');
			SHFILEOPSTRUCTW copyOperation
			{
				.wFunc = FO_COPY,
				.pFrom = path.data(),
				.pTo = path.data(),
				.fFlags = FOF_RENAMEONCOLLISION | FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI,
			};
			SHFileOperation(&copyOperation);
		}

		//Find the resulting file
		path = std::format(L"{}{}*.txt", tempDir, baseName);
		WIN32_FIND_DATA findData;
		if (wil::unique_hfind findHandle{ FindFirstFileW(path.data(), &findData) })
		{
			do
			{
				if (std::wstring_view foundName{ findData.cFileName }; foundName != fileName)
				{
					TempFileAutoDeleter{ std::format(L"{}{}", tempDir, foundName), false };
					return extractCopyString(fileName, foundName);
				}
			} while (FindNextFile(findHandle.get(), &findData));
		}

		return L"";
	}


	bool IsRenameSuffixValid(winrt::hstring const& suffix)
	{
		if (suffix.empty())
			return false;

		std::wstring stdText{suffix};
		//only contains empty char
		boost::trim(stdText);
		if (stdText.empty())
			return false;

		//contains invalid char
		constexpr static std::wstring_view illegal_chars{ L"<>:\"/\\|?*" };
		if (std::ranges::any_of(stdText, [](wchar_t c) {return illegal_chars.find(c) != std::wstring_view::npos; }))
			return false;

		return true;
	}

	void AddDestinationSuffixIfNeededForDirectory(std::filesystem::path const& source, std::filesystem::path& destination, std::wstring const& suffix)
	{
		//destination is already the final place, ie. has the folder name added to path
		if (!std::filesystem::exists(destination))
			return;

		//we have duplicate folders, add suffix
		//try: destination folder - Copy
		auto folderName = source.filename();
		auto destinationParent = destination.parent_path();

		if (auto firstCopyName = destinationParent / (folderName.wstring() + suffix).data(); !std::filesystem::exists(firstCopyName))
		{
			destination = std::move(firstCopyName);
			return;
		}

		//try: destination folder - Copy (N)
		for (int numberSuffix = 2;; ++numberSuffix)
		{
			if (auto numberedCopyName = destinationParent / std::format(L"{}{} ({})", folderName.wstring(), suffix, numberSuffix);
				!std::filesystem::exists(numberedCopyName))
			{
				destination = std::move(numberedCopyName);
				return;
			}
		}
	}

	void AddDestinationSuffixIfNeededForFile(std::filesystem::path const& source, std::filesystem::path& destination, std::wstring const& suffix)
	{
		if (!std::filesystem::exists(destination))
			return;

		//we have duplicate file, add suffix
		//try: destination - Copy.txt
		auto fileNameWithoutExtension = source.stem().wstring();
		auto const hasExtension = source.has_extension();
		auto destinationParent = destination.parent_path();
		auto const extension = source.extension().wstring();

		if (auto firstCopyName = destinationParent / (hasExtension ? std::format(L"{}{}{}", fileNameWithoutExtension, suffix, extension) : (fileNameWithoutExtension + suffix).data());
			!std::filesystem::exists(firstCopyName))
		{
			destination = std::move(firstCopyName);
			return;
		}

		//try: destination folder - Copy (N).txt
		for (int numberSuffix = 2;; ++numberSuffix)
		{
			if (auto numberedCopyName = destinationParent / (hasExtension ? std::format(L"{}{} ({}){}", fileNameWithoutExtension, suffix, numberSuffix, extension) : std::format(L"{}{} ({})", fileNameWithoutExtension, suffix, numberSuffix));
				!std::filesystem::exists(numberedCopyName))
			{
				destination = std::move(numberedCopyName);
				return;
			}
		}
	}

}