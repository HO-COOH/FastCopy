#include "pch.h"
#include "RobocopyInjectDll.h"
#include "KnownFolderPath.h"
#include <filesystem>
#include <array>

static auto GetCurrentExeFolder()
{
	std::array<wchar_t, MAX_PATH> buffer{};
	GetModuleFileName(nullptr, buffer.data(), buffer.size());
	return std::filesystem::path{ buffer.data() }.parent_path();
}

static auto DllDestinationFolder()
{
	return std::filesystem::path{
		KnwonFolderPath{ FOLDERID_LocalAppData, KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET }.Get()
	} / L"RobocopyEx";
}

std::wstring const& RobocopyInjectDll::Path()
{
	static auto result = []() {

		auto const dllDestFolder = DllDestinationFolder();

		if (!std::filesystem::exists(dllDestFolder))
			std::filesystem::create_directories(dllDestFolder);

		auto destPath = dllDestFolder / c_dllName;
		auto const dllPath = GetCurrentExeFolder() / c_dllName;

		bool shouldCopy = !std::filesystem::exists(destPath);
		if (!shouldCopy)
		{
			auto destTime = std::filesystem::last_write_time(destPath);
			auto srcTime = std::filesystem::last_write_time(dllPath);
			if (srcTime > destTime)
			{
				shouldCopy = true;
			}
		}

		if (shouldCopy)
		{
			/*
				When the app is installed to a non-system partition, Windows encrypts the package files.
				CopyFile (used by std::filesystem::copy_file) then tries to keep the copy encrypted as well which then fails with ERROR_ENCRYPTION_FAILED.
				COPY_FILE_ALLOW_DECRYPTED_DESTINATION lets the copy fall back to an unencrypted file instead.
				See: https://github.com/HO-COOH/FastCopy/issues/69#issuecomment-6604071818
			*/
			winrt::check_bool(CopyFileExW(
				dllPath.c_str(),
				destPath.c_str(),
				nullptr,
				nullptr,
				nullptr,
				COPY_FILE_ALLOW_DECRYPTED_DESTINATION
			));
		}

		return destPath.wstring();
	}();
	return result;
}

FARPROC RobocopyInjectDll::LoadLibraryAddr()
{
	static auto loadLibraryWAddr = GetProcAddress(
		GetModuleHandleW(L"kernel32.dll"),
		"LoadLibraryW"
	);
	return loadLibraryWAddr;
}
