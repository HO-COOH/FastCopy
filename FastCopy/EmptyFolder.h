#pragma once
#include <filesystem>

class EmptyFolder
{
	static std::filesystem::path getPath();
public:
	static std::filesystem::path const& GetOrCreate();
};