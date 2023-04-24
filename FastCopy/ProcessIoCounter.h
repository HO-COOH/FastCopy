#pragma once
#include <Windows.h>
#include <winnt.h>
#include <atomic>
class ProcessIoCounter
{
	IO_COUNTERS m_counter[2]{};
	IO_COUNTERS* m_counterPrev = &m_counter[0];
	IO_COUNTERS* m_counterCur = &m_counter[1];
	std::chrono::steady_clock::time_point m_lastUpdate{};
	std::atomic<HANDLE> m_handle{ INVALID_HANDLE_VALUE };
public:
	ProcessIoCounter() = default;
	ProcessIoCounter(HANDLE handle);
	
	void SetHandle(HANDLE handle) { m_handle = handle; }

	struct IOCounterDiff
	{
		ULONGLONG read;
		ULONGLONG write;
		std::chrono::nanoseconds duration;
	};

	IOCounterDiff Update();
};

