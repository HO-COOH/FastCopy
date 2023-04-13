#pragma once
#include <string>
#include "Env.h"
struct Config
{
public:
	static Config& GetInstance();
#ifdef _DEBUG
	size_t randomFileTotalBytes = 1024ull * 1024 * 128;
	size_t bigFileTotalBytes = 1024ull * 1024 * 256;
#else
	size_t randomFileTotalBytes = 1024ull * 1024 * 256;
	size_t bigFileTotalBytes = 1024ull * 1024 * 256;
#endif
	std::wstring sourceFolder = Env::GetFolderPath(Env::SpecialFolder::Desktop) + L"\\" + Env::GetRandomName();
	std::wstring destinationFolder = Env::GetTestDestinationPath(Env::GetRandomName(), randomFileTotalBytes);

	void CreateSourceAndDestinationFolder();
private:
	Config() = default;
	Config(Config const&) = delete;
	Config(Config&&) = default;
	Config& operator=(Config const&) = delete;
	Config& operator==(Config&&) = delete;
};

