#pragma once
#include "../SpeedTest/Process.h"
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <regex>
#include "RobocopyArgs.h"
#include <absl/strings/str_split.h>
#include <iostream>

//Forward declaration
struct RobocopyArgs;

class RobocopyArgsBuilder;

/**
 * @brief Represents a robocopy process
 */
class RobocopyProcess
{
	static inline boost::asio::io_service ios;
	constexpr static auto k_OutBufferSize = MAX_PATH + 20;


	boost::process::async_pipe pipeOut{ ios };
	boost::process::child m_child;
public:
	RobocopyProcess(RobocopyArgsBuilder const& builder, auto onProgress, auto onNewFile) :
		m_child{
			boost::process::cmd(boost::process::search_path("robocopy.exe").string() + " " + builder.Build()),
			boost::process::std_out > pipeOut
		}
	{
		static std::regex Progress{ "^[0-9]+[.]?[0-9]*%" };
		boost::asio::co_spawn(ios, [this, onProgress = std::move(onProgress), onNewFile = std::move(onNewFile)]()->boost::asio::awaitable<void>
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
							if (data.starts_with("New File"))
							{
								auto splited = absl::StrSplit(absl::string_view{ data.data(), data.length() }, "\t", absl::SkipEmpty());
								
								onNewFile(std::stoull((++splited.begin())->data()));
							}
							else if (std::regex_match(data.data(), data.data() + data.size(), Progress))
							{
								onProgress(std::strtof(data.data(), nullptr));
							}
						}
						buf.consume(n);
						auto c = buf.data().begin();
					}
				}
				catch (std::exception const& e)
				{
				}

			}, boost::asio::detached);
	}

	HANDLE Handle() { return m_child.native_handle(); }

	void WaitForExit()
	{
		ios.run();
		m_child.wait();
		std::cout << "Child exit with code: " << m_child.native_exit_code() << '\n';
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