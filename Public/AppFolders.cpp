#if __has_include("pch.h")
#include "pch.h"
#endif

#include <winrt/Windows.Storage.h>
#include "AppFolders.h"

static std::filesystem::path ensureFolderExists(std::filesystem::path path, bool ensure)
{
	if (ensure)
		std::filesystem::create_directories(path);
	return path;
}

std::filesystem::path AppFolders::localCacheFolder()
{
	return std::filesystem::path{ winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path().c_str() };
}

std::filesystem::path AppFolders::RecordsFolder(bool ensure)
{
	return ensureFolderExists(localCacheFolder() / L"Local" / L"Records", ensure);
}

std::filesystem::path AppFolders::ClipboardRecordsFolder(bool ensure)
{
	return ensureFolderExists(localCacheFolder() / L"Local" / L"ClipboardRecords", ensure);
}