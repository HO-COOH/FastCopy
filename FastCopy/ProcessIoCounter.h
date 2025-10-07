#pragma once
#include <Windows.h>
#include <winnt.h>
#include <vector>

/**
 * @brief A class for getting IO read/write bytes
 */
class ProcessIoCounter
{
	std::vector<std::pair<IO_COUNTERS, IO_COUNTERS>> m_counter{};
	std::chrono::steady_clock::time_point m_lastUpdate{};
	std::vector<HANDLE> m_handle;
	bool firstCounterAsCurrent = true;

	IO_COUNTERS& getCounterPrev(size_t i);
	IO_COUNTERS& getCounterCur(size_t i);
public:
	/**
	 * @brief Set a new process handle to the counter
	 * 
	 * @param handle a new process handle
	 */
	void SetHandle(HANDLE handle);

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

