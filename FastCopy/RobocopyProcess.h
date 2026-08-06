#pragma once
#include "../SpeedTest/Process.h"
#if __has_include("boost/process/v1/child.hpp")
#include <boost/process/v1.hpp>
#include <boost/process/v1/windows.hpp>
#else
#include <boost/process.hpp>
#include <boost/process/windows.hpp>
#endif
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <regex>
#include "RobocopyArgs.h"
#include "NewFile.h"
#include "NewDir.h"
#include "Same.h"
#include "Conflict.h"
#include "ExistingDir.h"
#include "ExtraDir.h"
#include "ExtraFile.h"
#include "Error.h"
#include "RobocopyExitCodes.h"
#include <iostream>
#include "RobocopyProcessStatus.h"
#include <wil/resource.h>
#include "CreateSuspend.h"
#include "CaptureThreadHandle.h"
#include "JobObject.h"

//Forward declaration
struct RobocopyArgs;

class RobocopyArgsBuilder;
/**
 * @brief Represents a robocopy process
 */
class RobocopyProcess
{
	static inline boost::asio::io_context ios;
	static inline auto work = boost::asio::make_work_guard(ios);
	constexpr static auto k_OutBufferSize = MAX_PATH + 20;

	wil::unique_handle m_hThread;
#if __has_include("boost/process/v1/child.hpp")
	boost::process::v1::async_pipe pipeOut{ ios };
	boost::process::v1::child m_child;
#else
	boost::process::async_pipe pipeOut{ ios };
	boost::process::child m_child;
#endif
	std::optional<Error> m_error;

	static void runContext();
	static std::regex& progressRegex();
	static JobObject& jobObjectInstance();

	static std::wstring const& robocopyPath();

	void injectProcess();
public:
	struct Exit {};
	struct InjectionFailed {};

	RobocopyProcess(RobocopyArgsBuilder const& builder, auto callbacks) :
		m_child
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
			create_suspend,
			CaptureThreadHandle{m_hThread}
		}
	{
		try
		{
			jobObjectInstance() << Handle();
			injectProcess();
		}
		catch (InjectionFailed const&)
		{

		}
		catch (...)
		{
			MessageBox(
				NULL,
				L"Robocopy injection failed! The software will not work correctly and please report an issue to the Github page of this repo.",
				L"RoboCopyEx",
				0
			);
		}
		SetConsoleCP(65001);
		SetConsoleOutputCP(65001);
		runContext();

		boost::asio::co_spawn(ios, [this, callbacks = std::move(callbacks)]()->boost::asio::awaitable<void>
		{
			std::vector<char> outBuf(k_OutBufferSize);           // that worked well for my decoding app.
			try
			{
				auto buf = boost::asio::dynamic_buffer(outBuf);
				while (true)
				{
					auto n = co_await boost::asio::async_read_until(pipeOut, buf, boost::regex{ "\r|\n" }, boost::asio::use_awaitable);
					std::string_view data{ outBuf.begin(), outBuf.begin() + n };
					std::cout << data << '\n';
					data.remove_prefix((std::min)(data.find_first_not_of(" \r\t"), data.size()));
					data.remove_suffix((std::min)(data.size() - 1 - data.find_last_not_of(" \r\n\t"), data.size()));
					if (!data.empty())
					{
						if (auto error = Error::TryParse(data))
						{
							//e.g. "2026/07/28 23:15:29 ERROR 5 (0x00000005) Accessing Destination Directory E:\"
							if (m_error) //flush a previous error that never got its description line
								callbacks(std::move(*m_error));
							m_error = std::move(error);
						}
						else if (m_error)
						{
							m_error->error.append(data); //append the description line (e.g. "Access is denied.")
							callbacks(std::move(*m_error));
							m_error.reset();
						}
						else if (data.starts_with(NewFile::Prefix))
						{
							if (auto newFile = NewFile::TryParse(data))
								callbacks(std::move(*newFile));
						}
						else if (data.starts_with(NewDir::Prefix))
						{
							if (auto newDir = NewDir::TryParse(data))
								callbacks(std::move(*newDir));
						}
						else if (data.starts_with(Same::Prefix))
						{
							if (auto same = Same::TryParse(data))
								callbacks(std::move(*same));
						}
						else if (std::ranges::any_of(Conflict::Prefix, [&data](auto prefix) {return data.starts_with(prefix); }))
						{
							if(auto conflict = Conflict::TryParse(data))
								callbacks(std::move(*conflict));
						}
						else if (auto existingDir = ExistingDir::TryParse(data))
						{
							callbacks(std::move(*existingDir));
						}
						else if (data.starts_with(ExtraDir::Prefix))
						{
							if (auto extraDir = ExtraDir::TryParse(data))
								callbacks(std::move(*extraDir));
						}
						else if (data.starts_with(ExtraFile::Prefix))
						{
							if (auto extraFile = ExtraFile::TryParse(data))
								callbacks(std::move(*extraFile));
						}
						else if (std::regex_match(data.data(), data.data() + data.size(), progressRegex()))
						{
							callbacks(std::strtof(data.data(), nullptr));
						}
						//else
						//{
						//	auto str = data;
						//}
					}
					buf.consume(n);
				}
			}
			catch (boost::system::system_error const& e)
			{
				auto const code = e.code();
				auto const childClosedPipe = code == boost::asio::error::eof || code == boost::asio::error::broken_pipe;
				if (!childClosedPipe)
				{
					//The read failed for a reason other than robocopy closing its stdout - typically the
					//overlapped read was aborted when the machine slept and woke (ERROR_OPERATION_ABORTED),
					//or the volume dropped out. We can no longer drain robocopy's output, so it would block
					//forever writing to a now-full pipe. Kill it so it does not hang, and report a synthetic
					//error so this item counts as failed - otherwise m_finishedFiles never reaches ItemCount()
					//and the whole operation stalls with the UI still responsive.
					std::error_code termEc;
					m_child.terminate(termEc);
					callbacks(Error{ "ERROR: copy interrupted (the system may have slept). Re-run to copy any remaining files." });
				}

				std::error_code waitEc;
				m_child.wait(waitEc);
				auto const rawExitCode = m_child.native_exit_code();
				auto const exitCode = static_cast<RoboCopyExitCodes>(rawExitCode);
				std::wcout << L"[robocopy] exit code " << rawExitCode << L": "
					<< GetRobocopyExitCodeDescription(exitCode) << L'\n';

				if (m_error) //flush a trailing error whose description line never arrived
				{
					callbacks(std::move(*m_error));
					m_error.reset();
				}
				//Always signal Exit so the view-model's finished-file accounting completes.
				callbacks(Exit{});
			}
			catch (std::exception const& e)
			{
				std::cerr << "[E]: " << e.what() << '\n';
			}
			//co_return;
		}, boost::asio::detached);


	}

	HANDLE Handle() const { return m_child.native_handle(); }

	void WaitForExit();
};

/**
 * @brief Represents the status of a invoke
 */
enum class Status
{
	Running,
	Pause,
	Cancel
};
