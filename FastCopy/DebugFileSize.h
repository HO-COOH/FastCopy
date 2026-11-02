#pragma once
#include <iostream>
#include <filesystem>

template<typename Path> //Path can be std::wstring_view or std::filesystem::path
void DebugFileSize(Path&& path)
{
	std::wcout << path << L": " << std::filesystem::file_size(path) << L'\n';
}
