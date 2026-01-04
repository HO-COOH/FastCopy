#include "pch.h"
#include "RobocopyProcess.h"
#include "RobocopyArgs.h"
#include "RobocopyInjectDll.h"
#include <wil/resource.h>
#include "Ntdll.h"

void RobocopyProcess::runContext()
{
	static bool v = [] {
		std::thread
		{
			[] 
			{
				ios.run();
			}
		}.detach();
		return true;
	}();
}

std::regex& RobocopyProcess::progressRegex()
{
	static std::regex Progress{ "^[0-9]+[.]?[0-9]*%" };
	return Progress;
}

void RobocopyProcess::injectProcess()
{
	auto& path = RobocopyInjectDll::Path();
	std::wcout << L"Injecting dll: " << path << L'\n';
	
	auto hProcess = m_child.native_handle();
	auto hThread = m_hThread.get();

	auto pathSize = (path.size() + 1) * sizeof(wchar_t);

	LPVOID remoteMem = winrt::check_pointer(VirtualAllocEx(
		hProcess,
		NULL,
		pathSize,
		MEM_COMMIT,
		PAGE_READWRITE
	));

	// WRITE DLL PATH
	winrt::check_bool(WriteProcessMemory(hProcess, remoteMem, path.c_str(), pathSize, NULL));

	// INJECT THE DLL (QueueUserAPC)
	// We use QueueUserAPC instead of CreateRemoteThread because the process is suspended.
	// CreateRemoteThread can deadlock if the loader lock is held or not initialized.
	// The APC will execute when the main thread resumes and initializes.
	auto loadLibraryAddr = RobocopyInjectDll::LoadLibraryAddr();
	if (loadLibraryAddr)
	{
		winrt::check_bool(QueueUserAPC(
			reinterpret_cast<PAPCFUNC>(loadLibraryAddr),
			hThread,
			reinterpret_cast<ULONG_PTR>(remoteMem)
		));
	}
	ResumeThread(hThread);
}

void RobocopyProcess::WaitForExit()
{
	ios.run();
	m_child.wait();
	std::cout << "Child exit with code: " << m_child.native_exit_code() << '\n';
}
