#pragma once
#include <Windows.h>
#include <string_view>
#include <variant>
#include <span>
#include <ranges>
#include <algorithm>

namespace internal
{
	static inline HANDLE CloseHandlesExceptForProcess(PROCESS_INFORMATION pi)
	{
		CloseHandle(pi.hThread);
		return pi.hProcess;
	}

	/**
	 * @brief Encapsulate a process handle, should be inherited by other class
	 */
	class ProcessBase
	{
	public:

		/**
		 * @brief Wait for the process handle to exit
		 * @return the exit code of the process
		 */
		[[maybe_unused]] int WaitForExit() const
		{
			WaitForSingleObject(m_processHandle, INFINITE);
			DWORD exitCode{};
			GetExitCodeProcess(m_processHandle, &exitCode);
			CloseHandle(m_processHandle);
			return exitCode;
		}

		[[nodiscard]] HANDLE Handle() const { return m_processHandle; }

		void Terminate()
		{
			TerminateProcess(m_processHandle, 1);
		}
	protected:
		HANDLE m_processHandle;
	};
}

template<typename Char>
class Process;

template<>
class Process<char> : public internal::ProcessBase
{
public:
	Process(std::string_view applicationName, std::string cmd = "")
	{
		STARTUPINFOA info{};
		info.cb = sizeof(info);
		if (!applicationName.empty() && cmd.find(applicationName) == std::string::npos)
		{
			cmd.insert(0, " ");
			cmd.insert(0, applicationName);
		}
		cmd.insert(0, applicationName);
		PROCESS_INFORMATION m_pi{};
		CreateProcessA(
			applicationName.data(),  //if string_view is empty(default constructed), data() will be nullptr, see https://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view
			cmd.data(),
			nullptr,
			nullptr,
			FALSE,
			0,
			nullptr,
			nullptr,
			&info,
			&m_pi
		);
		m_processHandle = internal::CloseHandlesExceptForProcess(m_pi);
	}
};

template<>
class Process<wchar_t> : public internal::ProcessBase
{
public:
	Process(std::wstring_view applicationName, std::wstring cmd = L"")
	{
		STARTUPINFOW info{};
		info.cb = sizeof(info);
		if (!applicationName.empty() && cmd.find(applicationName) == std::wstring::npos)
		{
			cmd.insert(0, L" ");
			cmd.insert(0, applicationName);
		}
		PROCESS_INFORMATION m_pi{};
		CreateProcessW(
			applicationName.data(),
			cmd.data(),
			nullptr,
			nullptr,
			FALSE,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&info,
			&m_pi
		);
		m_processHandle = internal::CloseHandlesExceptForProcess(m_pi);
	}
};

/**
 * Wait for all processes to finish
 * 
 * @param processes
 * @return Number of processes that successfully exit, that is the exit code of which is 0.
 */
template<typename Char>
int WaitForAllProcesses(std::span<Process<Char>> processes)
{
	return std::ranges::count_if(processes, [](auto const& process)
	{
		return process.WaitForExit() == 0;
	});
}
