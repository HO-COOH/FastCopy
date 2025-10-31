#include "pch.h"
#include "DebugFileSize.h"
#include <filesystem>
#include <format>
#include <iostream>

struct FileSizeInfo
{
	std::filesystem::path path;
	
	void Log() const
	{
		std::wcout <<
			std::format(
				L"{}: {}\n",
				path.wstring(),
				std::filesystem::file_size(path)
			);
	}
};

void DebugFileSize(std::wstring_view path)
{
	FileSizeInfo{ path }.Log();
}

void DebugFileSize(std::filesystem::path const& path)
{
	FileSizeInfo{ path }.Log();
}
