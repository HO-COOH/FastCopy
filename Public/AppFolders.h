#pragma once
#include <filesystem>

class AppFolders
{
	static std::filesystem::path localCacheFolder();
public:
	[[maybe_unused]] static std::filesystem::path RecordsFolder(bool ensure = false);
	static std::filesystem::path ClipboardRecordsFolder(bool ensure = false);
};