#include "pch.h"
#include "FileStats.h"
#include <filesystem>
#include <fstream>
#include <numeric>

static int32_t getNumFilesInFolder(std::string_view path)
{
	return std::distance(
		std::filesystem::recursive_directory_iterator{ path },
		std::filesystem::recursive_directory_iterator{}
	);
}

int32_t FileStats::GetNumFiles(winrt::hstring path)
{
	std::ifstream fs{ path.data() };
	if (!fs.is_open())
		return 0;

	std::string line;
	int32_t count{};
	while (std::getline(fs, line))
	{
		if (std::filesystem::is_directory(line))
			count += getNumFilesInFolder(line);
		else
			++count;
	}
	return count;
}
