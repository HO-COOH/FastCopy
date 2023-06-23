#include "pch.h"
#include "ProcessIoCounter.h"

ProcessIoCounter::ProcessIoCounter(HANDLE handle) : m_handle{handle}
{
}

void ProcessIoCounter::SetHandle(HANDLE handle)
{
	memset(m_counter, 0, sizeof(m_counter));
	m_handle = handle;
}

ProcessIoCounter::IOCounterDiff ProcessIoCounter::Update()
{
	if (!GetProcessIoCounters(m_handle, m_counterCur))
		return {};

	IOCounterDiff const diff
	{
		m_counterCur->ReadTransferCount - m_counterPrev->ReadTransferCount,
		m_counterCur->WriteTransferCount - m_counterPrev->WriteTransferCount,
		std::chrono::steady_clock::now() - m_lastUpdate
	};
	std::swap(m_counterPrev, m_counterCur);
	m_lastUpdate = std::chrono::steady_clock::now();
	return diff;
}

ProcessIoCounter::IOCounter ProcessIoCounter::GetTotal()
{
	return IOCounter{
		.read = m_counterCur->ReadTransferCount,
		.write = m_counterCur->WriteTransferCount
	};
}

ProcessIoCounter::IOCounter& ProcessIoCounter::IOCounter::operator+=(IOCounter rhs)
{
	read += rhs.read; 
	write += rhs.write;
	return *this;
}

ProcessIoCounter::IOCounter ProcessIoCounter::IOCounter::operator+(IOCounter rhs)
{
	auto copy = *this;
	copy += rhs;
	return copy;
}
