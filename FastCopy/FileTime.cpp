#include "pch.h"
#include "FileTime.h"
#include <stdexcept>
#include <wil/resource.h>

FileTime::FileTime(HANDLE fileHandle)
{
	if (!GetFileTime(fileHandle, &createTime, &lastAccessTime, &lastWriteTime))
		throw std::runtime_error{"Failed to GetFileTime"};
}

FileTime::FileTime(std::wstring_view path) : 
	FileTime{ wil::unique_handle{CreateFile(path.data(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)}.get() }
{
}
