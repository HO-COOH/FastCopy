#include "pch.h"
#include "RobocopyProcess.h"
#include "RobocopyArgs.h"
#include "Ntdll.h"
#include "KnownFolderPath.h"
#if __has_include("boost/process/v1/child.hpp")
#include <boost/process/v1.hpp>
#include <boost/process/v1/windows.hpp>
#else
#include <boost/process.hpp>
#include <boost/process/windows.hpp>
#endif
#include <boost/asio/executor_work_guard.hpp>
#include <filesystem>
#include <format>
#include <thread>
#include "ProcessInjectHandler.hpp"

RobocopyProcess::RobocopyProcess(RobocopyArgsBuilder const& builder)
	: pipeOut{ ioContext() }
	, m_child
	{
#if __has_include("boost/process/v1/child.hpp")
		boost::process::v1::cmd(robocopyPath() + L" " + builder.Build()),
		boost::process::v1::std_out > pipeOut,
		boost::process::v1::windows::create_no_window,
#else
		boost::process::cmd(robocopyPath() + L" " + builder.Build()),
		boost::process::std_out > pipeOut,
		boost::process::windows::create_no_window,
#endif
		ProcessInjectHandler{}
	}
{
}

boost::asio::io_context& RobocopyProcess::ioContext()
{
	struct SharedIoContext
	{
		boost::asio::io_context ios;
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work{ boost::asio::make_work_guard(ios) };

		SharedIoContext()
		{
			std::thread{ [this] { ios.run(); } }.detach();
		}
	};

	static SharedIoContext context;
	return context.ios;
}

std::wstring const& RobocopyProcess::robocopyPath()
{
	static std::wstring const path = std::wstring{ KnwonFolderPath{ FOLDERID_System }.Get() } + L"\\robocopy.exe";
	return path;
}

std::string RobocopyProcess::describeInjectionExitCode(DWORD exitCode)
{
	switch (static_cast<RobocopyInjectionHandshake::InjectionStatus>(exitCode))
	{
		case RobocopyInjectionHandshake::InjectionStatus::ForceEnglishFailed:
			return "Robocopy injection failed: unable to set English UI language in robocopy.exe.";
		case RobocopyInjectionHandshake::InjectionStatus::HookWfopenFailed:
			return "Robocopy injection failed: unable to install Unicode job-file (_wfopen) hook in robocopy.exe.";
		case RobocopyInjectionHandshake::InjectionStatus::SetConsoleUtf8Failed:
			return "Robocopy injection failed: unable to configure UTF-8 output in robocopy.exe.";
		default:
			return std::format(
				"Robocopy injection failed: RobocopyInjection.dll did not initialize (exit code 0x{:08X}). It may have been blocked by antivirus software.",
				exitCode
			);
	}
}

std::regex const& RobocopyProcess::progressRegex()
{
	static std::regex const Progress{ "^[0-9]+[.]?[0-9]*%" };
	return Progress;
}

JobObject& RobocopyProcess::JobObjectInstance()
{
	static JobObject s_instance;
	return s_instance;
}

HANDLE RobocopyProcess::Handle() const
{
	return m_child.native_handle();
}

void RobocopyProcess::Terminate() noexcept
{
	if (!m_child.valid())
		return;

	std::error_code ec;
	m_child.terminate(ec);
}

void RobocopyProcess::Suspend() const
{
	// boost::process sets handle to INVALID_HANDLE_VALUE after wait()
	// before passing to NtSuspendProcess, it must be checked, otherwise main process freeze
	if (m_child.valid())
		NtDll::NtSuspendProcess(Handle());
}

void RobocopyProcess::Resume() const
{
	if (m_child.valid())
		NtDll::NtResumeProcess(Handle());
}
