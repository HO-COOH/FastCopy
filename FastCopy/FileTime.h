#pragma once
#include <Windows.h>
#include <string_view>
#include <winrt/Windows.Foundation.h>

/**
 * @brief Wrapper for FILETIME
 */
struct FileTime
{
	FILETIME createTime;
	FILETIME lastAccessTime;
	FILETIME lastWriteTime;

	/**
	 * @brief Get file time by handle
	 * 
	 * @param fileHandle
	 */
	FileTime(HANDLE fileHandle);

	/**
	 * @brief Get file time by path
	 * 
	 * @param path
	 */
	FileTime(std::wstring_view path);

};

/**
 * @brief winrt::clock wrapper for `FileTime`
 */
class WinRTFileTime : private FileTime
{
public:
	/**
	 * @brief Get file time by handle
	 *
	 * @param fileHandle
	 */
	WinRTFileTime(HANDLE fileHandle);

	/**
	 * @brief Get file time by path
	 * 
	 * @param path
	 */
	WinRTFileTime(std::wstring_view path);


	winrt::Windows::Foundation::DateTime CreateTime();
	winrt::Windows::Foundation::DateTime LastAccessTime();
	winrt::Windows::Foundation::DateTime LastWriteTime();
};
