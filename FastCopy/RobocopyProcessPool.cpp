#include "pch.h"
#include "RobocopyProcessPool.h"

void RobocopyProcessPool::Preallocate(size_t count)
{
	m_statuses = std::vector<RobocopyProcessStatus>(count); //using assignment because RobocopyProcessStatus is non-movable
	m_remainingProcesses.store(count, std::memory_order_release);
	m_statusCount.store(count, std::memory_order_release);
}

ProcessStatus RobocopyProcessPool::GetStatus() const noexcept
{
	return m_status.load(std::memory_order_acquire);
}

void RobocopyProcessPool::SetStatus(ProcessStatus status) noexcept
{
	m_status.store(status, std::memory_order_release);
}

bool RobocopyProcessPool::WaitIfPaused() const noexcept
{
	m_status.wait(ProcessStatus::Pause, std::memory_order_acquire);
	return m_status.load(std::memory_order_acquire) == ProcessStatus::Running;
}

bool RobocopyProcessPool::AcquireSlot() noexcept
{
	while (true)
	{
		m_status.wait(ProcessStatus::Pause, std::memory_order_acquire);
		auto const st = m_status.load(std::memory_order_acquire);
		if (st == ProcessStatus::Cancel || st == ProcessStatus::Failed)
			return false;

		auto const active = m_activeProcesses.load(std::memory_order_acquire);
		if (active < k_MaxConcurrentProcesses)
		{
			m_activeProcesses.fetch_add(1, std::memory_order_acq_rel);
			return true;
		}
		if (active & k_SchedulerStopFlag)
			continue;

		m_activeProcesses.wait(active, std::memory_order_acquire);
	}
}

void RobocopyProcessPool::CancelUnlaunched(size_t count) noexcept
{
	m_remainingProcesses.fetch_sub(count, std::memory_order_acq_rel);
}

bool RobocopyProcessPool::OnProcessFinished() noexcept
{
	m_activeProcesses.fetch_sub(1, std::memory_order_acq_rel);
	m_activeProcesses.notify_one();
	return m_remainingProcesses.fetch_sub(1, std::memory_order_acq_rel) == 1;
}

void RobocopyProcessPool::stopScheduling() noexcept
{
	m_activeProcesses.fetch_or(k_SchedulerStopFlag, std::memory_order_release);
	m_activeProcesses.notify_all();
}

void RobocopyProcessPool::registerLaunchedProcess(size_t index, RobocopyProcess& process)
{
	std::lock_guard lock{ m_processMutex };
	auto const st = m_status.load(std::memory_order_acquire);
	if (st == ProcessStatus::Failed)
		process.Terminate();
	else if (st != ProcessStatus::Running)
		process.Suspend();
	m_launchedCount = index + 1;
}

void RobocopyProcessPool::Pause()
{
	m_status.store(ProcessStatus::Pause, std::memory_order_release);
	suspendAll();
}

void RobocopyProcessPool::Resume()
{
	m_status.store(ProcessStatus::Running, std::memory_order_release);
	resumeAll();
	m_status.notify_all();
}

bool RobocopyProcessPool::Cancel()
{
	if (m_status.load(std::memory_order_acquire) == ProcessStatus::Failed)
		return false;

	m_status.store(ProcessStatus::Cancel, std::memory_order_release);
	m_status.notify_all();
	stopScheduling();
	suspendAll();
	return true;
}

void RobocopyProcessPool::Fail()
{
	m_status.store(ProcessStatus::Failed, std::memory_order_release);
	m_status.notify_all();
	stopScheduling();
	terminateAll();
}

void RobocopyProcessPool::suspendAll()
{
	std::lock_guard lock{ m_processMutex };
	for (size_t i = 0; i < m_launchedCount; ++i)
	{
		if (auto& process = m_statuses[i].m_process)
			process->Suspend();
	}
}

void RobocopyProcessPool::resumeAll()
{
	std::lock_guard lock{ m_processMutex };
	for (size_t i = 0; i < m_launchedCount; ++i)
	{
		if (auto& process = m_statuses[i].m_process)
			process->Resume();
	}
}

void RobocopyProcessPool::terminateAll()
{
	std::lock_guard lock{ m_processMutex };
	for (size_t i = 0; i < m_launchedCount; ++i)
	{
		if (auto& process = m_statuses[i].m_process)
			process->Terminate();
	}
}

uint64_t RobocopyProcessPool::GetInFlightCopiedBytes() const noexcept
{
	auto const count = m_statusCount.load(std::memory_order_acquire);
	uint64_t sum = 0;
	for (size_t i = 0; i < count; ++i)
		sum += m_statuses[i].m_copiedBytes.load(std::memory_order_relaxed);
	return sum;
}
