#include "pch.h"
#include "UIThreadWrapper.h"
#include <ppltasks.h>

UIThreadWrapper& UIThreadWrapper::operator=(winrt::Microsoft::UI::Dispatching::DispatcherQueue queue)
{
	assert(m_queue == nullptr);
	m_queue = queue;
	while (!m_tasks.empty())
	{
		auto task = std::move(m_tasks.front());
		m_tasks.pop();
		m_queue.TryEnqueue(task);
	}
	return *this;
}
