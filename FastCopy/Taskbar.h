#pragma once
#include <ShObjIdl_core.h>

/**
 * @brief Wrapper for controlling taskbar icon progress
 */
class Taskbar
{
	static ITaskbarList3* getPtr();
public:

	/**
	 * @brief Set taskbar icon progress state of a window
	 * 
	 * @param hwnd window handle
	 */
	static void SetProgressState(HWND hwnd, TBPFLAG state);

	/**
	 * @brief Set taskbar icon progress value
	 * 
	 * @param hwnd window handle
	 * @param current current value, default to the number of percentage if `maximum` not specified
	 * @param maximum default = `100`
	 */
	static void SetProgressValue(HWND hwnd, ULONGLONG current, ULONGLONG maximum = 100);
};

