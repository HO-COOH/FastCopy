#pragma once
#include <Windows.h>
#include <winnt.h>

/**
 * @brief A class for getting IO read/write bytes
 */
class ProcessIoCounter
{
	IO_COUNTERS m_counter[2]{};
	IO_COUNTERS* m_counterPrev = &m_counter[0];
	IO_COUNTERS* m_counterCur = &m_counter[1];
	std::chrono::steady_clock::time_point m_lastUpdate{};
	HANDLE m_handle{ INVALID_HANDLE_VALUE };
public:
	ProcessIoCounter() = default;

	/**
	 * @brief Initialize with a handle to process
	 * 
	 * @param handle handle to the process
	 */
	ProcessIoCounter(HANDLE handle);

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

		IOCounter& operator+=(IOCounter rhs);
		IOCounter operator+(IOCounter rhs);
	};

	struct IOCounterDiff : IOCounter
	{
		std::chrono::nanoseconds duration;
	};

	/**
	 * @brief Return the amount of IO difference and time difference to last call
	 */
	IOCounterDiff Update();

	/**
	 * @brief Get current total
	 */
	IOCounter GetTotal();
};

