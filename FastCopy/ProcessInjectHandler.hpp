#pragma once
#if __has_include("boost/process/v1/child.hpp")
#include <boost/process/v1/extend.hpp>
#else
#include <boost/process/extend.hpp>
#endif
#include "RobocopyProcess.h"
#include <format>

class ProcessInjectHandler :
#if __has_include("boost/process/v1/extend.hpp")
	public boost::process::v1::extend::handler
#else
	public boost::process::extend::handler
#endif
{
	static bool hasEnglishResource();
	static std::wstring const& robocopyPath();
	static void injectProcess(HANDLE hProcess, HANDLE hThread);
public:
	using InjectionFailed = RobocopyProcess::InjectionFailed;

	void on_setup(auto& exec) const
	{
		if (static auto englishResource = hasEnglishResource())
		{
			exec.creation_flags |= CREATE_SUSPENDED;
			return;
		}

		throw InjectionFailed{
			"robocopy.exe does not have English resources (en-US\\robocopy.exe.mui is missing). Please check your Windows installation."
		};
	}

	void on_success(auto& exec) const
	{
		try
		{
			RobocopyProcess::JobObjectInstance() << exec.proc_info.hProcess;
			injectProcess(exec.proc_info.hProcess, exec.proc_info.hThread);
		}
		catch (InjectionFailed&)
		{
			throw;
		}
		catch (winrt::hresult_error const& e)
		{
			throw InjectionFailed{
				std::format("Robocopy injection failed (0x{:08X}): {}", static_cast<uint32_t>(e.code()), winrt::to_string(e.message()))
			};
		}
		catch (std::exception const& e)
		{
			throw InjectionFailed{ std::format("Robocopy injection failed: {}", e.what()) };
		}
		catch (...)
		{
			throw InjectionFailed{ "Robocopy injection failed! Please report an issue to the GitHub page of this repo." };
		}
	}
};