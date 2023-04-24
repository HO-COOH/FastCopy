#include "pch.h"
#include "ProcessIoCounter.h"

ProcessIoCounter::ProcessIoCounter(HANDLE handle) : m_handle{handle}
{
}

ProcessIoCounter::IOCounterDiff ProcessIoCounter::Update()
{
	if (!GetProcessIoCounters(m_handle, m_counterCur))
		return {};

	IOCounterDiff const diff
	{
		.read = m_counterCur->ReadTransferCount - m_counterPrev->ReadTransferCount,
		.write = m_counterCur->WriteTransferCount - m_counterPrev->WriteTransferCount,
		.duration = std::chrono::steady_clock::now() - m_lastUpdate
	};
	std::swap(m_counterPrev, m_counterCur);
	m_lastUpdate = std::chrono::steady_clock::now();
	return diff;
}
