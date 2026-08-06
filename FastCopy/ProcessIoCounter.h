#pragma once
#include <Windows.h>
#include <winnt.h>
#include <vector>

/**
 * @brief A class for getting IO read/write bytes
 */
class ProcessIoCounter
{
	IO_COUNTERS m_prev{};
	std::chrono::steady_clock::time_point m_lastUpdate{};
public:
	struct IOCounter
	{
		ULONGLONG read;
		ULONGLONG write;
	};

	struct IOCounterDiff : IOCounter
	{
		std::chrono::nanoseconds duration;
	};

	/**
	 * @brief Return the amount of IO difference and time difference to last call
	 */
	IOCounterDiff Update();
};

