#pragma once
#include <winrt/Microsoft.UI.Dispatching.h>
#include "ProcessIoCounter.h"

template<typename Self>
class ProcessIOUpdater
{
	auto& getSelf()
	{
		return static_cast<Self&>(*this);
	}

	ProcessIoCounter m_counter;
	winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer m_timer{ nullptr };
public:
	void SetHandle(HANDLE handle)
	{
		m_counter.SetHandle(handle);
	}

	template<typename Duration>
	void Start(Duration duration, winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcherQueue)
	{
		m_timer = dispatcherQueue.CreateTimer();
		m_timer.Interval(duration);
		m_timer.Tick(
			[this](auto&&...)
			{
				getSelf().OnUpdateCopySpeed(m_counter.Update());
			}
		);
		m_timer.Start();
	}

	void Stop()
	{
		m_timer.Stop();
	}

	~ProcessIOUpdater()
	{
		Stop();
	}
};
