#include "pch.h"
#include "EmptyFolder.h"
#include <winrt/Windows.Storage.h>

std::filesystem::path EmptyFolder::getPath()
{
	return std::filesystem::path{ winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path().data() } / "EmptyFolder";
}

std::filesystem::path const& EmptyFolder::GetOrCreate()
{
	static auto path = getPath();
	if (!std::filesystem::exists(path))
		std::filesystem::create_directories(path);

	return path;
}