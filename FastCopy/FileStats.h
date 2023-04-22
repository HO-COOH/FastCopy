#pragma once

#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.FileProperties.h>

namespace FileStats
{
	inline winrt::Windows::Foundation::IAsyncOperation<uint64_t> GetFileSize(winrt::Windows::Storage::IStorageItem folder)
	{
		auto property = co_await folder.GetBasicPropertiesAsync();
		co_return property.Size();
	}

	int32_t GetNumFiles(winrt::hstring path);


};

