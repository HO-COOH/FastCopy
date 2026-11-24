#pragma once
#if __has_include("boost/process/v1/child.hpp")
#include <boost/process/v1/async_pipe.hpp>
#include <boost/process/v1/child.hpp>
#else
#include <boost/process/async_pipe.hpp>
#include <boost/process/child.hpp>
#endif
#include <boost/asio/awaitable.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include "NewFile.h"
#include "NewDir.h"
#include "Same.h"
#include "Conflict.h"
#include "ExistingDir.h"
#include "ExtraDir.h"
#include "ExtraFile.h"
#include "Error.h"
#include "RobocopyExitCodes.h"
#include "JobObject.h"
#include "MatchLineBreak.hpp"
#include "../Public/RobocopyInjectionHandshake.h"

class RobocopyArgsBuilder;

/**
 * @brief Represents a robocopy process
 */
class RobocopyProcess
{
public:
	struct Exit {};
	struct InjectionFailed
	{
		std::string message;
	};

private:
	constexpr static auto k_OutBufferSize = MAX_PATH + 20;

#if __has_include("boost/process/v1/child.hpp")
	boost::process::v1::async_pipe pipeOut;
	boost::process::v1::child m_child;
#else
	boost::process::async_pipe pipeOut;
	boost::process::child m_child;
#endif

	static boost::asio::io_context& ioContext();
	static std::regex const& progressRegex();
	static std::wstring const& robocopyPath();
	static std::string describeInjectionExitCode(DWORD exitCode);

	static constexpr std::string_view trimLine(std::string_view line) noexcept
	{
		auto const first = line.find_first_not_of(" \r\n\t");
		if (first == std::string_view::npos)
			return {};
		auto const last = line.find_last_not_of(" \r\n\t");
		return line.substr(first, last - first + 1);
	}

	RobocopyProcess(RobocopyArgsBuilder const& builder);

	static void flushPendingError(std::optional<Error>& pendingError, auto& callbacks)
	{
		if (pendingError)
		{
			callbacks(std::move(*pendingError));
			pendingError.reset();
		}
	}

	static void dispatchLine(std::string_view data, std::optional<Error>& pendingError, auto&& callbacks)
	{
		if (auto error = Error::TryParse(data))
		{
			// e.g. "2026/07/28 23:15:29 ERROR 5 (0x00000005) Accessing Destination Directory E:\"
			flushPendingError(pendingError, callbacks);
			if (error->isSingleLine)
				callbacks(std::move(*error));
			else
				pendingError = std::move(error);
		}
		else if (pendingError)
		{
			pendingError->error.push_back(' ');
			pendingError->error.append(data); // append the description line (e.g. "Access is denied.")
			flushPendingError(pendingError, callbacks);
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
		else if (std::ranges::any_of(Conflict::Prefix, [&data](auto prefix) { return data.starts_with(prefix); }))
		{
			if (auto conflict = Conflict::TryParse(data))
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
	}

	boost::asio::awaitable<void> monitorOutput(auto callbacks)
	{
		bool injectionVerified = false;
		bool injectionReported = false;
		bool childClosedPipe = false;
		std::optional<Error> pendingError;

		std::vector<char> outBuf;
		outBuf.reserve(k_OutBufferSize);

		try
		{
			auto buf = boost::asio::dynamic_buffer(outBuf);
			while (true)
			{
				auto const n = co_await boost::asio::async_read_until(pipeOut, buf, MatchLineBreak{}, boost::asio::use_awaitable);
				std::string_view const rawLine{ outBuf.data(), n };
				std::cout << rawLine << '\n';

				auto const data = trimLine(rawLine);

				if (!data.empty())
				{
					if (injectionVerified)
						dispatchLine(data, pendingError, callbacks);
					else
					{
						if (data == RobocopyInjectionHandshake::ReadyMarker)
							injectionVerified = true;
						else
						{
							// Output arrived from robocopy.exe without the DllMain handshake marker first:
							// LoadLibraryW failed or was blocked, so stop robocopy immediately before it runs unhooked.
							Terminate();
							injectionReported = true;
							callbacks(InjectionFailed{
								"Robocopy injection failed: RobocopyInjection.dll was not loaded into robocopy.exe (it may have been blocked by antivirus software)."
							});
							break;
						}

					}
				}

				buf.consume(n);
			}
		}
		catch (boost::system::system_error const& e)
		{
			auto const code = e.code();
			childClosedPipe = (code == boost::asio::error::eof || code == boost::asio::error::broken_pipe);
			if (!childClosedPipe)
			{
				// The read failed for a reason other than robocopy closing its stdout - typically the
				// overlapped read was aborted when the machine slept and woke (ERROR_OPERATION_ABORTED),
				// or the volume dropped out. We can no longer drain robocopy's output, so it would block
				// forever writing to a now-full pipe. Kill it so it does not hang, and report a synthetic
				// error so this item counts as failed - otherwise m_finishedFiles never reaches ItemCount()
				// and the whole operation stalls with the UI still responsive.
				Terminate();
				flushPendingError(pendingError, callbacks);
				callbacks(Error{ "ERROR: copy interrupted (the system may have slept). Re-run to copy any remaining files." });
			}
		}
		catch (std::exception const& e)
		{
			std::cerr << "[E]: " << e.what() << '\n';
			Terminate();
		}

		std::error_code waitEc;
		m_child.wait(waitEc);

		if (!injectionReported)
		{
			auto const rawExitCode = static_cast<DWORD>(m_child.native_exit_code());
			auto const exitCode = static_cast<RoboCopyExitCodes>(rawExitCode);
			std::wcout << L"[robocopy] exit code " << rawExitCode << L": "
				<< GetRobocopyExitCodeDescription(exitCode) << L'\n';

			if (!injectionVerified && childClosedPipe)
			{
				callbacks(InjectionFailed{ describeInjectionExitCode(rawExitCode) });
			}
		}

		flushPendingError(pendingError, callbacks);
		callbacks(Exit{});
	}

public:
	static JobObject& JobObjectInstance();

	RobocopyProcess(RobocopyArgsBuilder const& builder, auto&& callbacks) : RobocopyProcess{ builder }
	{
		boost::asio::co_spawn(
			ioContext(),
			monitorOutput(std::move(callbacks)),
			boost::asio::detached
		);
	}

	HANDLE Handle() const;

	/**
	 * @brief Suspends the process. Does nothing if it has already exited.
	 */
	void Suspend() const;

	/**
	 * @brief Resumes the process. Does nothing if it has already exited.
	 */
	void Resume() const;

	/**
	 * @brief Terminates the process if still running.
	 */
	void Terminate() noexcept;
};
