#pragma once
#include <ShObjIdl_core.h>

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

	static void SetProgressState(HWND hwnd, ProgressState state);

	static void SetProgressValue(HWND hwnd, ULONGLONG current, ULONGLONG maximum = 100);
};

