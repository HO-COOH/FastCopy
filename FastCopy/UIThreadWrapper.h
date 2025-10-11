#pragma once
#include <winrt/Microsoft.UI.Dispatching.h>
#include <queue>
#include <functional>
class UIThreadWrapper
{
	std::queue<std::function<void()>> m_tasks;
public:
	winrt::Microsoft::UI::Dispatching::DispatcherQueue m_queue{ nullptr };
	
	UIThreadWrapper& operator=(winrt::Microsoft::UI::Dispatching::DispatcherQueue const& queue);

	template<typename Func>
	void TryEnqueue(Func&& f)
	{
		if (m_queue)
			m_queue.TryEnqueue(std::forward<Func>(f));
		else
			m_tasks.push(std::forward<Func>(f));
	}
};

