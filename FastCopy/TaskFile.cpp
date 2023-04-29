#include "pch.h"
#include "TaskFile.h"
#include <fstream>
#include <filesystem>

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

	std::wifstream fs{ m_path.data() };
	if (!fs.is_open())
		return 0;

	std::wstring line;
	int32_t count{};
	while (std::getline(fs, line))
	{
		if (std::filesystem::is_directory(line))
			count += getNumFilesInFolder(line);
		else
			++count;
		lines.push_back(std::move(line));
	}
	numFiles.resize(lines.size());
	totalFiles = count;
	return count;
}

int32_t TaskFile::GetNumFiles(int index)
{
	if (numFiles[index].has_value())
		return *numFiles[index];

	numFiles[index] = 
		std::filesystem::is_directory(lines[index]) ? getNumFilesInFolder(lines[index]) : 1;
	return *numFiles[index];
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
