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
#include <iostream>
#include "RobocopyProcessStatus.h"
#include <wil/resource.h>
#include "CreateSuspend.h"
#include "CaptureThreadHandle.h"

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
	static void runContext();
	static std::regex& progressRegex();

	void injectProcess();
public:
	struct Exit {};

	RobocopyProcess(RobocopyArgsBuilder const& builder, auto callbacks) :
		m_child
		{
#if __has_include("boost/process/v1/child.hpp")
			boost::process::v1::cmd(boost::process::v1::search_path("robocopy.exe").wstring() + L" " + builder.Build()),
			boost::process::v1::std_out > pipeOut,
			boost::process::v1::windows::create_no_window,
#else	
			boost::process::cmd(boost::process::search_path("robocopy.exe").wstring() + L" " + builder.Build()),
			boost::process::std_out > pipeOut,
			boost::process::windows::create_no_window,
#endif
			create_suspend,
			CaptureThreadHandle{m_hThread}
		}
	{
		try
		{
			injectProcess();
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
			auto callbacksCopy = std::move(callbacks);
			auto thisCopy = this;

			std::vector<char> outBuf(k_OutBufferSize);           // that worked well for my decoding app.
			try
			{
				auto buf = boost::asio::dynamic_buffer(outBuf);
				while (true)
				{
					auto n = co_await boost::asio::async_read_until(thisCopy->pipeOut, buf, boost::regex{ "\r|\n" }, boost::asio::use_awaitable);
					std::string_view data{ outBuf.begin(), outBuf.begin() + n };
					std::cout << data << '\n';
					data.remove_prefix((std::min)(data.find_first_not_of(" \r\t"), data.size()));
					data.remove_suffix((std::min)(data.size() - 1 - data.find_last_not_of(" \r\n\t"), data.size()));
					if (!data.empty())
					{
						if (data.starts_with(NewFile::Prefix))
						{
							if (auto newFile = NewFile::TryParse(data))
								callbacksCopy(std::move(*newFile));
						}
						else if (data.starts_with(NewDir::Prefix))
						{
							if (auto newDir = NewDir::TryParse(data))
								callbacksCopy(std::move(*newDir));
						}
						else if (data.starts_with(Same::Prefix))
						{
							if (auto same = Same::TryParse(data))
								callbacksCopy(std::move(*same));
						}
						else if (std::ranges::any_of(Conflict::Prefix, [&data](auto prefix) {return data.starts_with(prefix); }))
						{
							if(auto conflict = Conflict::TryParse(data))
								callbacksCopy(std::move(*conflict));
						}
						else if (auto existingDir = ExistingDir::TryParse(data))
						{
							callbacksCopy(std::move(*existingDir));
						}
						else if (data.starts_with(ExtraDir::Prefix))
						{
							if (auto extraDir = ExtraDir::TryParse(data))
							{
								//...
							}
						}
						else if (data.starts_with(ExtraFile::Prefix))
						{
							if (auto extraFile = ExtraFile::TryParse(data))
								callbacksCopy(std::move(*extraFile));
						}
						else if (std::regex_match(data.data(), data.data() + data.size(), progressRegex()))
						{
							callbacksCopy(std::strtof(data.data(), nullptr));
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
				//auto category = e.code().category();
				if (auto code = e.code(); code == boost::asio::error::eof || code == boost::asio::error::broken_pipe)
					callbacksCopy(Exit{});
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

	//RobocopyProcess(RobocopyProcess&&) noexcept = default;
	~RobocopyProcess()
	{
		//OutputDebugString(L"Exited\n");
	}
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

//https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/robocopy#exit-return-codes
enum class RoboCopyExitCodes
{
	//No files were copied. No failure was encountered. No files were mismatched. The files already exist in the destination directory; therefore, the copy operation was skipped.
	NoCopy = 0,

	//All files were copied successfully.
	Success = 1,

	//There are some additional files in the destination directory that aren't present in the source directory. No files were copied.
	NoCopyExtraFiles = 2,

	//Some files were copied. Additional files were present. No failure was encountered.
	PartialCopyExtraFiles = 3,

	//Some files were copied. Some files were mismatched. No failure was encountered.
	PartialCopyMismatchFiles = 5,

	//Additional files and mismatched files exist. No files were copied and no failures were encountered meaning that the files already exist in the destination directory.
	NoCopyExtraFilesAndMismatchFiles = 6,

	//Files were copied, a file mismatch was present, and additional files were present.
	PartialCopyExtraFilesAndMismatchedFiles = 7,

	//Several files didn't copy.
	PartialError = 8
};