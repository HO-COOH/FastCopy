#include "pch.h"
#include "WinRTFileTime.h"


WinRTFileTime::WinRTFileTime(HANDLE fileHandle) : FileTime{ fileHandle }
{
}

WinRTFileTime::WinRTFileTime(std::wstring_view path) : FileTime{ path }
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
