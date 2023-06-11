#pragma once
#include <Windows.h>
class MutexWrapper
{
	HANDLE m_handle{ INVALID_HANDLE_VALUE };
public:
	/**
	 * @brief Create or opens a mutex
	 */
	MutexWrapper(wchar_t const* name, bool owning = true) : m_handle{ CreateMutex(nullptr, owning, name) } {}
	
	/**
	 * @brief Try lock the mutex, returns immediately whether it's successful or not
	 */
	bool TryLock();

	/**
	 * @brief Release mutex if owned
	 */
	~MutexWrapper();
};

