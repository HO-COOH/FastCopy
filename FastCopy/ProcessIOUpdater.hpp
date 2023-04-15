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
	template<typename Duration>
	void Start(Duration duration)
	{
		winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
			[this](auto)
			{
				getSelf().OnUpdate(m_counter.Update());
			}, duration
		);
	}


};
