#pragma once
#include <winrt/Windows.Foundation.h>
#include "FileTime.h"

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
