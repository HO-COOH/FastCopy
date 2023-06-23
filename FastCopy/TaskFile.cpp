#include "pch.h"
#include "TaskFile.h"
#include <fstream>
#include <filesystem>
#include <codecvt>
#include "FileWrapper.h"
#include <numeric>

static int32_t getNumFilesInFolder(std::wstring_view path)
{
	return std::distance(
		std::filesystem::recursive_directory_iterator{ path },
		std::filesystem::recursive_directory_iterator{}
	);
}

static uint64_t getFileSizeInFolder(std::wstring_view path)
{
	return std::accumulate(
		std::filesystem::recursive_directory_iterator{path},
		std::filesystem::recursive_directory_iterator{},
		0ull,
		[](uint64_t lhs, std::filesystem::directory_entry p)
		{
			return lhs + std::filesystem::file_size(p);
		}
	);
}

uint64_t TaskFile::GetSizeOfPath(std::wstring_view path)
{
	return std::filesystem::is_directory(path) ?
		getFileSizeInFolder(path) : std::filesystem::file_size(path);
}

int32_t TaskFile::GetNumFiles()
{
	if (!lines.empty())
		return totalFiles;

	FileWrapper fs{ _wfopen(m_path.data(), L"rb") };
	if (!fs)
		return 0;

	int32_t count{};
	while (true)
	{
		size_t length{};
		if (!fs.read(&length, sizeof(length), 1))
			break;

		std::wstring line(length, {});
		
		if (!fs.read(line.data(), 2, length))
			break;

		if (std::filesystem::is_directory(line))
			numFiles.push_back(getNumFilesInFolder(line));
		else
			numFiles.push_back(1);
		count += numFiles.back();
		lines.push_back(std::move(line));
	}
	numFiles.resize(lines.size());
	totalFiles = count;
	return count;
}

int32_t TaskFile::GetNumFiles(int index)
{
	return numFiles[index];
}

uint64_t TaskFile::GetTotalSize()
{
	return std::accumulate(
		lines.begin(),
		lines.end(),
		0ull,
		[](uint64_t result, std::wstring const& line)
		{
			return result + GetSizeOfPath(line);
		}
	);
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

