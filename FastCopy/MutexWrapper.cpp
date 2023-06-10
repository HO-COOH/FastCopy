#include "pch.h"
#include "MutexWrapper.h"

bool MutexWrapper::TryLock()
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return false;

	auto const waitResult = WaitForSingleObject(m_handle, 0);
	return waitResult == WAIT_OBJECT_0;
}

MutexWrapper::~MutexWrapper()
{
	if (m_handle != INVALID_HANDLE_VALUE)
	{
		ReleaseMutex(m_handle);
		CloseHandle(m_handle);
	}
}
