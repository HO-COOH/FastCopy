#include "pch.h"
#include "TaskFile.h"
#include <fstream>
#include <filesystem>
#include <codecvt>
#include "FileWrapper.h"

static int32_t getNumFilesInFolder(std::wstring_view path)
{
	return std::distance(
		std::filesystem::recursive_directory_iterator{ path },
		std::filesystem::recursive_directory_iterator{}
	);
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
