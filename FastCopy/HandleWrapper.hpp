#pragma once

#include <Windows.h>

/**
 * @brief RAII wrapper for `HANDLE`
 */
class HandleWrapper
{
	HANDLE m_handle;
public:
	HandleWrapper(HANDLE handle) : m_handle{ handle } {}

	//Disable copy, move only
	HandleWrapper(HandleWrapper const&) = delete;
	HandleWrapper& operator=(HandleWrapper const&) = delete;
	HandleWrapper(HandleWrapper&&) noexcept = default;
	HandleWrapper& operator=(HandleWrapper&&) noexcept = default;

	HANDLE Get() const { return m_handle; }

	~HandleWrapper()
	{
		CloseHandle(m_handle);
	}
};
