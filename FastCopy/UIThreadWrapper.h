#pragma once
#include <winrt/Microsoft.UI.Dispatching.h>
#include <queue>
#include <functional>
class UIThreadWrapper
{
	winrt::Microsoft::UI::Dispatching::DispatcherQueue m_queue{ nullptr };
	std::queue<std::function<void()>> m_tasks;
public:

	UIThreadWrapper& operator=(winrt::Microsoft::UI::Dispatching::DispatcherQueue queue);

	template<typename Func>
	void TryEnqueue(Func&& f)
	{
		if (m_queue)
			m_queue.TryEnqueue(std::forward<Func>(f));
		else
			m_tasks.push(std::forward<Func>(f));
	}
};

