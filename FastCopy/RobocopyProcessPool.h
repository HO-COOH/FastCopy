#pragma once
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <utility>
#include <vector>
#include "RobocopyProcessStatus.h"

enum class ProcessStatus
{
	Running,
	Pause,
	Cancel,
	Failed
};

class RobocopyProcessPool
{
public:
	constexpr static uint32_t k_MaxConcurrentProcesses = 8;

	void Preallocate(size_t count);

	[[nodiscard]] ProcessStatus GetStatus() const noexcept;
	void SetStatus(ProcessStatus status) noexcept;
	[[nodiscard]] bool WaitIfPaused() const noexcept;

	[[nodiscard]] bool AcquireSlot() noexcept;
	void CancelUnlaunched(size_t count) noexcept;
	[[nodiscard]] bool OnProcessFinished() noexcept;

	template<typename Callbacks>
	void Launch(size_t index, RobocopyArgsBuilder const& args, Callbacks&& callbacks)
	{
		auto& proc = m_statuses[index].m_process.emplace(args, std::forward<Callbacks>(callbacks));
		registerLaunchedProcess(index, proc);
	}

	void Pause();
	void Resume();
	bool Cancel();
	void Fail();

	[[nodiscard]] uint64_t GetInFlightCopiedBytes() const noexcept;

	[[nodiscard]] constexpr RobocopyProcessStatus& operator[](size_t index) noexcept { return m_statuses[index]; }
	[[nodiscard]] constexpr RobocopyProcessStatus const& operator[](size_t index) const noexcept { return m_statuses[index]; }

private:
	constexpr static uint32_t k_SchedulerStopFlag = 1u << 31;

	void registerLaunchedProcess(size_t index, RobocopyProcess& process);
	void stopScheduling() noexcept;
	void suspendAll();
	void resumeAll();
	void terminateAll();

	std::mutex m_processMutex;
	std::vector<RobocopyProcessStatus> m_statuses;
	size_t m_launchedCount{};
	std::atomic_size_t m_statusCount{};
	std::atomic_uint32_t m_activeProcesses{};
	std::atomic<ProcessStatus> m_status{ ProcessStatus::Running };
	std::atomic_size_t m_remainingProcesses{};
};
