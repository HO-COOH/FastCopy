#include "pch.h"
#include "RenameUtils.h"
#include <chrono>
#include <wil/resource.h>
#include <shellapi.h>
#include <filesystem>
#include "Settings.h"

class TempFileAutoDeleter
{
	std::wstring_view m_tempFile;
public:
	TempFileAutoDeleter(std::wstring_view tempFile, bool create) : m_tempFile{tempFile}
	{
		if (create)
		{
			wil::unique_handle
			{ CreateFileW(
				tempFile.data(),
				GENERIC_WRITE,
				0,
				nullptr,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			) };
		}
	}
	
	~TempFileAutoDeleter()
	{
		DeleteFile(m_tempFile.data());
	}
};


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

static void addDestinationSuffixIfNeededForDirectory(std::filesystem::path const& source, std::filesystem::path& destination, std::wstring const& suffix)
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
	for (int numberSuffix = 2;;++numberSuffix)
	{
		if (auto numberedCopyName = destinationParent / std::format(L"{}{} ({})", folderName.wstring(), suffix, numberSuffix);
			!std::filesystem::exists(numberedCopyName))
		{
			destination = std::move(numberedCopyName);
			return;
		}
	}
}

namespace Utils
{
	std::wstring GetDefaultRenameSuffix()
	{
		//create a test.txt file
		wchar_t temp[MAX_PATH]{};
		GetTempPath(MAX_PATH, temp);

		auto baseName = std::to_wstring(std::chrono::system_clock::now().time_since_epoch().count());
		auto fileName = std::format(L"{}.txt", baseName);
		auto path = std::format(L"{}{}", temp, fileName);

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
		path = std::format(L"{}{}*.txt", temp, baseName);
		WIN32_FIND_DATA findData;
		if (wil::unique_hfind findHandle{ FindFirstFileW(path.data(), &findData) })
		{
			do
			{
				if (std::wstring_view foundName{ findData.cFileName }; foundName != fileName)
				{
					//TempFileAutoDeleter{findData.pa}
					return extractCopyString(fileName, foundName);
				}
			} while (FindNextFile(findHandle.get(), &findData));
		}

		return L"";
	}

	void AddDestinationSuffixIfNeeded(bool isDirectory, 
		std::filesystem::path const& source, 
		std::filesystem::path& destination)
	{
		static std::wstring const suffix{ Settings{}.Get<winrt::hstring>(Settings::RenameSuffix) };
		assert(isDirectory);
		addDestinationSuffixIfNeededForDirectory(source, destination, suffix);
	}
}