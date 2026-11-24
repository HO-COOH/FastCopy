#include "pch.h"
#include "ProcessInjectHandler.hpp"
#include <filesystem>
#include <string>
#include "KnownFolderPath.h"
#include "RobocopyInjectDll.h"

std::wstring const& ProcessInjectHandler::robocopyPath()
{
	static std::wstring const path = std::wstring{ KnwonFolderPath{ FOLDERID_System }.Get() } + L"\\robocopy.exe";
	return path;
}

bool ProcessInjectHandler::hasEnglishResource()
{
	auto const muiPath = std::filesystem::path{ robocopyPath() }.parent_path() / L"en-US" / L"robocopy.exe.mui";
	std::error_code ec;
	return std::filesystem::exists(muiPath, ec) && !ec;
}

void ProcessInjectHandler::injectProcess(HANDLE hProcess, HANDLE hThread)
{
	auto const& path = RobocopyInjectDll::Path();
	std::wcout << L"Injecting dll: " << path << L'\n';

	auto const pathSize = (path.size() + 1) * sizeof(wchar_t);

	LPVOID remoteMem = winrt::check_pointer(VirtualAllocEx(
		hProcess,
		NULL,
		pathSize,
		MEM_COMMIT,
		PAGE_READWRITE
	));

	winrt::check_bool(WriteProcessMemory(hProcess, remoteMem, path.c_str(), pathSize, NULL));

	// We use QueueUserAPC instead of CreateRemoteThread because the process is suspended.
	// CreateRemoteThread can deadlock if the loader lock is held or not initialized.
	// The APC will execute when the main thread resumes and initializes.
	auto const loadLibraryAddr = RobocopyInjectDll::LoadLibraryAddr();
	if (!loadLibraryAddr)
	{
		throw InjectionFailed{ "Robocopy injection failed: could not resolve kernel32.dll!LoadLibraryW." };
	}

	winrt::check_bool(QueueUserAPC(
		reinterpret_cast<PAPCFUNC>(loadLibraryAddr),
		hThread,
		reinterpret_cast<ULONG_PTR>(remoteMem)
	));

	if (ResumeThread(hThread) == static_cast<DWORD>(-1))
	{
		winrt::throw_last_error();
	}
}