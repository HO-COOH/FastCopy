#pragma once
#include <winrt/Windows.System.Threading.h>
#include "ProcessIoCounter.h"

template<typename Self>
class ProcessIOUpdater
{
	auto& getSelf()
	{
		return static_cast<Self&>(*this);
	}

	ProcessIoCounter m_counter;
	winrt::Windows::System::Threading::ThreadPoolTimer m_timer{ nullptr };
public:
	void SetHandle(HANDLE handle)
	{
		m_counter.SetHandle(handle);
	}

	template<typename Duration>
	void Start(Duration duration)
	{
		m_timer = winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
			[this](auto)
			{
				getSelf().OnUpdateCopySpeed(m_counter.Update());
			}, duration
		);
	}

	void Stop()
	{
		m_timer.Cancel();
	}

	~ProcessIOUpdater()
	{
		Stop();
	}
};
