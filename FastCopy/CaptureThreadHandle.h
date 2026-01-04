#pragma once
#if __has_include("boost/process/v1/extend.hpp")
#include <boost/process/v1/extend.hpp>
#else
#include <boost/process/extend.hpp>
#endif
#include <wil/resource.h>
#include <Windows.h>

/**
* @brief This class is needed for capturing the `PROCESS_INFORMATION`, because `boost::process` (v1 to be exact)
		 does not have a getter for it
*/
struct CaptureThreadHandle : 
#if __has_include("boost/process/v1/extend.hpp")
	boost::process::v1::extend::handler
#else
	boost::process::extend::handler
#endif
{
	wil::unique_handle& m_hThread;
	CaptureThreadHandle(wil::unique_handle& hThread) : m_hThread(hThread) {}

	void on_success(auto& executor) const
	{
		HANDLE hThread;
		DuplicateHandle(
			GetCurrentProcess(),
			executor.proc_info.hThread,
			GetCurrentProcess(),
			&hThread,
			0,
			FALSE,
			DUPLICATE_SAME_ACCESS
		);
		m_hThread.reset(hThread);
	}
};

