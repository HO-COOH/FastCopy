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
	ProcessIoCounter::IOCounter m_total{};
public:
	void SetHandle(HANDLE handle)
	{
		m_total += m_counter.GetTotal();
		m_counter.SetHandle(handle);
	}

	template<typename Duration>
	void Start(Duration duration)
	{
		winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
			[this](auto)
			{
				getSelf().OnUpdateCopySpeed(m_counter.Update());
			}, duration
		);
	}

	auto GetTotal()
	{
		return m_total + m_counter.GetTotal();
	}
};
