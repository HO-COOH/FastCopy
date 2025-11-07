#pragma once
#include <Windows.h>
#include <string_view>


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
