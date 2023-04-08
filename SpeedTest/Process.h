#pragma once
#include <Windows.h>
#include <string_view>
#include <variant>
#include <span>

namespace internal
{
	class ProcessBase
	{
	public:
		[[maybe_unused]] int WaitForExit() const
		{
			WaitForSingleObject(m_pi.hProcess, INFINITE);
			DWORD exitCode{};
			GetExitCodeProcess(m_pi.hProcess, &exitCode);
			CloseHandle(m_pi.hProcess);
			CloseHandle(m_pi.hThread);
			return exitCode;
		}
	protected:
		HANDLE m_handle = INVALID_HANDLE_VALUE;
		PROCESS_INFORMATION m_pi;
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
		m_startupInfo.cb = sizeof(m_startupInfo);
		if (!applicationName.empty())
		{
			cmd.insert(0, " ");
			cmd.insert(0, applicationName);
		}
		cmd.insert(0, applicationName);
		CreateProcessA(
			applicationName.data(),
			cmd.data(),
			nullptr,
			nullptr,
			FALSE,
			0,
			nullptr,
			nullptr,
			&m_startupInfo,
			&m_pi
		);
	}
private:
	STARTUPINFOA m_startupInfo{};
};

template<>
class Process<wchar_t> : public internal::ProcessBase
{
public:
	Process(std::wstring_view applicationName, std::wstring cmd = L"")
	{
		m_startupInfo.cb = sizeof(m_startupInfo);
		if (!applicationName.empty())
		{
			cmd.insert(0, L" ");
			cmd.insert(0, applicationName);
		}
		CreateProcessW(
			applicationName.data(),
			cmd.data(),
			nullptr,
			nullptr,
			FALSE,
			CREATE_NO_WINDOW,
			nullptr,
			nullptr,
			&m_startupInfo,
			&m_pi
		);
	}
private:
	STARTUPINFOW m_startupInfo{};
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
