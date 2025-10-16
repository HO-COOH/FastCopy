#include "pch.h"
#include "ProcessIoCounter.h"


IO_COUNTERS& ProcessIoCounter::getCounterPrev(size_t i)
{
	return firstCounterAsCurrent ? m_counter[i].second : m_counter[i].first;
}

IO_COUNTERS& ProcessIoCounter::getCounterCur(size_t i)
{
	return firstCounterAsCurrent ? m_counter[i].first : m_counter[i].second;
}

void ProcessIoCounter::SetHandle(HANDLE handle)
{
	m_handle.emplace_back(handle);
	m_counter.emplace_back();
}

ProcessIoCounter::IOCounterDiff ProcessIoCounter::Update()
{
	IOCounterDiff diff{};

	for (size_t i = 0; i < m_counter.size(); ++i)
	{
		auto& counterCur = getCounterCur(i);
		if (!GetProcessIoCounters(m_handle[i], &counterCur))
			continue;

		auto& counterPrev = getCounterPrev(i);
		diff.read += counterCur.ReadTransferCount - counterPrev.ReadTransferCount;
		diff.write += counterCur.WriteTransferCount - counterPrev.WriteTransferCount;
	}

	firstCounterAsCurrent = !firstCounterAsCurrent;
	auto const now = std::chrono::steady_clock::now();
	diff.duration = now - m_lastUpdate;
	m_lastUpdate = now;
	return diff;
}
