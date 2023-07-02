#include "pch.h"
#include "FileTime.h"
#include "HandleWrapper.hpp"
#include <stdexcept>

FileTime::FileTime(HANDLE fileHandle)
{
	if (!GetFileTime(fileHandle, &createTime, &lastAccessTime, &lastWriteTime))
		throw std::runtime_error{"Failed to GetFileTime"};
}

FileTime::FileTime(std::wstring_view path) : FileTime{ HandleWrapper{CreateFile(path.data(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)}.Get()}
{
}

WinRTFileTime::WinRTFileTime(HANDLE fileHandle) : FileTime{fileHandle}
{
}

WinRTFileTime::WinRTFileTime(std::wstring_view path) : FileTime{path}
{
}

winrt::Windows::Foundation::DateTime WinRTFileTime::CreateTime()
{
	return winrt::clock::from_FILETIME(createTime);
}

winrt::Windows::Foundation::DateTime WinRTFileTime::LastAccessTime()
{
	return winrt::clock::from_FILETIME(lastAccessTime);
}

winrt::Windows::Foundation::DateTime WinRTFileTime::LastWriteTime()
{
	return winrt::clock::from_FILETIME(lastWriteTime);
}
