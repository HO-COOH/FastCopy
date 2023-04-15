#pragma once
#include <Windows.h>
#include <winnt.h>
class ProcessIoCounter
{
public:
	struct IOCounterDiff
	{
		ULONGLONG read;
		ULONGLONG write;
	};

	IOCounterDiff Update();
};

