#pragma once
#include <string>
#include "Env.h"
struct Config
{
public:
	static Config& GetInstance();
#ifdef _DEBUG
	size_t randomFileTotalBytes = 1024ull * 1024 * 128;
#else
	size_t randomFileTotalBytes = 1024ull * 1024 * 1024 * 4;
#endif
	std::wstring sourceFolder = Env::GetFolderPath(Env::SpecialFolder::Desktop) + Env::GetRandomName();
	std::wstring destinationFolder = Env::GetTestDestinationPath(Env::GetRandomName(), randomFileTotalBytes);
private:
	Config() = default;
	Config(Config const&) = delete;
	Config(Config&&) = default;
	Config& operator=(Config const&) = delete;
	Config& operator==(Config&&) = delete;
};

