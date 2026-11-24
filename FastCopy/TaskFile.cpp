#include "pch.h"
#include "TaskFile.h"
#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <wil/resource.h>

static constexpr CopyOperation parseOperation(std::wstring_view path)
{
	auto const slashPos = path.find_last_of(LR"(\/)");
	auto const filename = (slashPos == std::wstring_view::npos) ? path : path.substr(slashPos + 1);
	if (filename.empty())
		return CopyOperation::Copy;

	switch (filename[0])
	{
		case L'C': return CopyOperation::Copy;
		case L'M': return CopyOperation::Move;
		case L'D': return CopyOperation::Delete;
		default:
			throw std::runtime_error{ "Invalid operation" };
	}
}

static constexpr bool isCurrentOrParentFolder(wchar_t const* name) noexcept
{
	return name[0] == L'.' && (name[1] == L'\0' || (name[1] == L'.' && name[2] == L'\0'));
}

static constexpr bool isSymlinkOrJunction(WIN32_FIND_DATAW const& findData) noexcept
{
	return (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
		(findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK || findData.dwReserved0 == IO_REPARSE_TAG_MOUNT_POINT);
}

static constexpr uint64_t getFileSize(WIN32_FIND_DATAW const& findData) noexcept
{
	return (static_cast<uint64_t>(findData.nFileSizeHigh) << 32) | findData.nFileSizeLow;
}

static constexpr uint64_t getFileSize(WIN32_FILE_ATTRIBUTE_DATA const& attrData) noexcept
{
	return (static_cast<uint64_t>(attrData.nFileSizeHigh) << 32) | attrData.nFileSizeLow;
}

struct FolderScanner
{
	std::wstring path;
	WIN32_FIND_DATAW findData{};
	TaskFile::TaskCount result{};
	bool countDirs{};

	void Scan()
	{
		if (path.back() != L'\\' && path.back() != L'/')
			path.push_back(L'\\');
		auto const baseLen = path.size();
		path.push_back(L'*');

		wil::unique_hfind findHandle{ FindFirstFileExW(
			path.c_str(),
			FindExInfoBasic,
			&findData,
			FindExSearchNameMatch,
			nullptr,
			FIND_FIRST_EX_LARGE_FETCH
		) };
		if (!findHandle)
			return;

		do
		{
			auto const* name = findData.cFileName;
			if (isCurrentOrParentFolder(name))
				continue;

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (countDirs)
					++result.totalFiles;

				if (isSymlinkOrJunction(findData))
					continue;
				
				path.resize(baseLen);
				path.append(name);
				Scan();
			}
			else
			{
				++result.totalFiles;
				result.totalBytes += getFileSize(findData);
			}
		} while (FindNextFileW(findHandle.get(), &findData));
	}
};

static TaskFile::TaskCount getFileCountAndSizeInFolder(std::wstring_view rootPath, bool countDirs)
{
	if (rootPath.empty())
		return {};

	FolderScanner scanner{ .countDirs = countDirs };
	scanner.path.reserve(MAX_PATH);
	scanner.path.assign(rootPath);
	scanner.Scan();
	return scanner.result;
}

static std::unique_ptr<std::byte[]> readFileToBuffer(HANDLE file, size_t totalBytes)
{
	auto buffer = std::make_unique_for_overwrite<std::byte[]>(totalBytes);

	size_t bytesReadTotal = 0;
	while (bytesReadTotal < totalBytes)
	{
		auto const chunk = static_cast<DWORD>((std::min<size_t>)(totalBytes - bytesReadTotal, MAXDWORD));
		DWORD bytesRead = 0;
		winrt::check_bool(ReadFile(file, buffer.get() + bytesReadTotal, chunk, &bytesRead, nullptr) && bytesRead != 0);
		bytesReadTotal += bytesRead;
	}

	return buffer;
}

void TaskFile::parseItems(std::span<std::byte const> buffer)
{
	size_t offset = 0;
	while (buffer.size() - offset >= sizeof(size_t))
	{
		size_t length{};
		std::memcpy(&length, buffer.data() + offset, sizeof(length));
		offset += sizeof(length);

		if (length > (buffer.size() - offset) / sizeof(wchar_t))
			break;

		auto const* chars = reinterpret_cast<wchar_t const*>(buffer.data() + offset);
		m_items.push_back(TaskItem{ .path = std::wstring{ chars, length } });
		offset += length * sizeof(wchar_t);
	}
}

TaskFile::TaskFile(winrt::hstring const& path) :
	m_path{ path },
	m_operation{ parseOperation(path) }
{
	if (m_path.empty())
		return;

	wil::unique_hfile file{ CreateFileW(
		m_path.data(),
		GENERIC_READ | DELETE,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_DELETE_ON_CLOSE,
		nullptr
	) };
	winrt::check_bool(static_cast<bool>(file));

	LARGE_INTEGER fileSize{};
	winrt::check_bool(GetFileSizeEx(file.get(), &fileSize));
	if (fileSize.QuadPart <= 0)
		return;

	auto const totalBytes = static_cast<size_t>(fileSize.QuadPart);
	auto const buffer = readFileToBuffer(file.get(), totalBytes);
	parseItems({ buffer.get(), totalBytes });
}

TaskFile::TaskCount TaskFile::ComputeCountAndSize()
{
	assert(!m_computed);

	TaskFile::TaskCount result{};
	if (m_items.empty())
		return result;

	m_computed = true;
	auto const isDelete = m_operation == CopyOperation::Delete;
	for (auto& item : m_items)
	{
		WIN32_FILE_ATTRIBUTE_DATA attrData{};
		auto const hasAttr = GetFileAttributesExW(item.path.c_str(), GetFileExInfoStandard, &attrData);
		if (hasAttr && (attrData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			auto const [folderCount, folderSize] = getFileCountAndSizeInFolder(item.path, isDelete);
			auto const entryCount = isDelete ? folderCount + 1 : folderCount;
			item.isDirectory = true;
			item.numFiles = entryCount;
			item.size = folderSize;
			result.totalBytes += folderSize;
			result.totalFiles += entryCount;
			continue;
		}

		auto const fileSize = hasAttr ? getFileSize(attrData) : 0ull;
		item.isDirectory = false;
		item.numFiles = 1;
		item.size = fileSize;
		result.totalBytes += fileSize;
		++result.totalFiles;
	}

	return result;
}
