#include "pch.h"
#include "ProcessIoCounter.h"
#include "RobocopyProcess.h"

ProcessIoCounter::IOCounterDiff ProcessIoCounter::Update()
{
	auto const now = std::chrono::steady_clock::now();
	auto const nowIo = RobocopyProcess::JobObjectInstance().GetAccountingInfo().IoInfo;

	IOCounterDiff diff
	{
		IOCounter 
		{
			.read = nowIo.ReadTransferCount - m_prev.ReadTransferCount,
			.write = nowIo.WriteTransferCount - m_prev.WriteTransferCount
		},
		now - m_lastUpdate
	};
	m_lastUpdate = now;
	m_prev = nowIo;
	return diff;
}
