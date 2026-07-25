#include "pch.h"
#include "TaskFile.h"
#include <fstream>
#include <filesystem>
#include <codecvt>
#include "FileWrapper.h"
#include <numeric>
#include "DebugFileSize.h"
#include <iostream>

static int32_t countInFolder(std::wstring_view path, auto predicate)
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
	for (; !ec && it != end; it.increment(ec))
	{
		if (predicate(*it))
			++count;
	}
	return count;
}

static int32_t getNumFilesInFolder(std::wstring_view path)
{
	return countInFolder(path, [](std::filesystem::directory_entry const& entry)
	{
		std::error_code ec;
		return entry.is_regular_file(ec);
	});
}

//Counts both regular files and sub-directories. Used for delete, where robocopy /MIR
//reports every file (*EXTRA File) and every sub-folder (*EXTRA Dir) it removes.
static int32_t getNumItemsInFolder(std::wstring_view path)
{
	return countInFolder(path, [](std::filesystem::directory_entry const& entry)
	{
		std::error_code ec;
		return entry.is_regular_file(ec) || entry.is_directory(ec);
	});
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

int32_t TaskFile::GetNumFiles()
{
	if (!lines.empty())
		return totalFiles;

	int32_t count{};
	{
		FileWrapper fs{ _wfopen(m_path.data(), L"rb") };
		if (!fs)
			return 0;

		auto const isDelete = GetOperation() == CopyOperation::Delete;

		while (true)
		{
			size_t length{};
			if (!fs.read(&length, sizeof(length), 1))
				break;

			std::wstring line(length, {});

			if (!fs.read(line.data(), 2, length))
				break;

			if (std::filesystem::is_directory(line))
			{
				//For delete, robocopy reports every sub-folder (*EXTRA Dir) alongside every
				//file, and we delete the folder itself afterwards. Count them all (files +
				//sub-folders + the folder itself) so the progress denominator matches what is
				//actually processed - and so an empty / folder-only directory is not 0 items.
				if (isDelete)
					numFiles.push_back(getNumItemsInFolder(line) + 1);
				else
					numFiles.push_back(getNumFilesInFolder(line));
			}
			else
				numFiles.push_back(1);
			count += numFiles.back();
			lines.push_back(std::move(line));
		}
		numFiles.resize(lines.size());
		totalFiles = count;
	}

	try
	{
		std::filesystem::remove(m_path.data());
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << '\n';
	}
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

