#include "pch.h"
#include "TaskFile.h"
#include <fstream>
#include <filesystem>
#include <codecvt>
#include "FileWrapper.h"
#include <numeric>
#include "DebugFileSize.h"
#include <iostream>
#include <utility>

static std::pair<int32_t, uint64_t> getFileCountAndSizeInFolder(std::wstring_view path, bool countDirs)
{
	std::error_code ec;
	std::filesystem::recursive_directory_iterator it
	{
		path,
		std::filesystem::directory_options::skip_permission_denied,
		ec
	};
	std::filesystem::recursive_directory_iterator const end;

	int32_t count{};
	uint64_t size{};
	for (; !ec && it != end; it.increment(ec))
	{
		std::error_code entryEc;
		if (it->is_regular_file(entryEc))
		{
			++count;
			size += it->file_size(entryEc);
		}
		else if (countDirs && it->is_directory(entryEc))
			++count;
	}
	return { count, size };
}

static uint64_t getFileSizeInFolder(std::wstring_view path)
{
	return std::accumulate(
		std::filesystem::recursive_directory_iterator{path},
		std::filesystem::recursive_directory_iterator{},
		0ull,
		[](uint64_t lhs, std::filesystem::directory_entry p)
		{
			/*DebugFileSize(p);*/
			return lhs + (p.is_directory() ? 0 : std::filesystem::file_size(p));
		}
	);
}


uint64_t TaskFile::GetSizeOfPath(std::wstring_view path)
{
	auto const isDir = std::filesystem::is_directory(path);
#if (defined DEBUG) || (defined _DEBUG)
	if (!isDir)
		DebugFileSize(path);
#endif

	return isDir ?
		getFileSizeInFolder(path) : std::filesystem::file_size(path);
}

TaskFile::TaskFile(winrt::hstring const& path) : m_path{ path }
{
	{
		FileWrapper fs{ _wfopen(m_path.data(), L"rb") };
		if (!fs)
			return;

		while (true)
		{
			size_t length{};
			if (!fs.read(&length, sizeof(length), 1))
				break;

			std::wstring line(length, {});

			if (!fs.read(line.data(), 2, length))
				break;

			lines.push_back(std::move(line));
		}
	}

	try
	{
		std::filesystem::remove(m_path.data());
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << '\n';
	}
}


void TaskFile::ComputeCountAndSize()
{
	if (!numFiles.empty() || lines.empty())
		return; //already computed, or nothing was loaded

	auto const isDelete = GetOperation() == CopyOperation::Delete;
	int32_t count{};
	uint64_t size{};
	numFiles.reserve(lines.size());
	for (auto const& line : lines)
	{
		if (std::filesystem::is_directory(line))
		{
			auto const [folderCount, folderSize] = getFileCountAndSizeInFolder(line, isDelete);
			numFiles.push_back(isDelete ? folderCount + 1 : folderCount);
			size += folderSize;
		}
		else
		{
			numFiles.push_back(1);
			std::error_code ec;
			size += std::filesystem::file_size(line, ec);
		}
		count += numFiles.back();
	}
	totalFiles = count;
	totalSize = size;
}

int32_t TaskFile::GetNumFiles()
{
	ComputeCountAndSize();
	return totalFiles;
}

int32_t TaskFile::GetNumFiles(int index)
{
	return numFiles[index];
}

uint64_t TaskFile::GetTotalSize()
{
	ComputeCountAndSize();
	return totalSize;
}

int TaskFile::IndexOf(TaskFileIterator<typename std::vector<std::wstring>::iterator> const& iter)
{
	return std::distance(lines.begin(), iter.get());
}

CopyOperation TaskFile::GetOperation() const
{
	switch (std::filesystem::path{ m_path.data() }.filename().wstring()[0])
	{
		case L'C': return CopyOperation::Copy;
		case L'M': return CopyOperation::Move;
		case L'D': return CopyOperation::Delete;
		default:
			throw std::runtime_error{ "Invalid operation" };
	}
}

