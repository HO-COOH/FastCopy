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
public:
	void SetHandle(HANDLE handle)
	{
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
		return m_counter.GetTotal();
	}
};
