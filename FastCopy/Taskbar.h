#pragma once
#include <ShObjIdl_core.h>

/**
 * @brief Wrapper for controlling taskbar icon progress
 */
class Taskbar
{
	static ITaskbarList3* getPtr();
public:
	enum class ProgressState
	{
		NoProgress = TBPF_NOPROGRESS,
		Indeterminate = TBPF_INDETERMINATE,
		Normal = TBPF_NORMAL,
		Error = TBPF_ERROR,
		Paused = TBPF_PAUSED
	};

	/**
	 * @brief Set taskbar icon progress state of a window
	 * 
	 * @param hwnd window handle
	 */
	static void SetProgressState(HWND hwnd, ProgressState state);

	/**
	 * @brief Set taskbar icon progress value
	 * 
	 * @param hwnd window handle
	 * @param current current value, default to the number of percentage if `maximum` not specified
	 * @param maximum default = `100`
	 */
	static void SetProgressValue(HWND hwnd, ULONGLONG current, ULONGLONG maximum = 100);
};

