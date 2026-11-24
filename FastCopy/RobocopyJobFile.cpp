#include "pch.h"
#include "RobocopyJobFile.h"
#include "KnownFolderPath.h"
#include <atomic>
#include <chrono>
#include <format>
#include <numeric>
#include <wil/resource.h>

static std::filesystem::path const& GetJobFolder()
{
	static std::filesystem::path const folder = [] {
		auto path = std::filesystem::path{
			KnwonFolderPath{ FOLDERID_LocalAppData, KF_FLAG_RETURN_FILTER_REDIRECTION_TARGET }.Get()
		} / L"RobocopyEx" / L"Jobs";
		std::error_code ec;
		std::filesystem::create_directories(path, ec);

		// Opportunistically clean up stale .rcj files (> 1 hour old) left behind by killed processes
		auto const cutoff = std::filesystem::file_time_type::clock::now() - std::chrono::hours{ 1 };
		for (std::filesystem::directory_iterator it{ path, ec }; !ec && it != std::filesystem::directory_iterator{}; it.increment(ec))
		{
			std::error_code entryEc;
			if (!it->is_regular_file(entryEc) || entryEc)
				continue;
			if (it->last_write_time(entryEc) >= cutoff || entryEc)
				continue;

			std::filesystem::remove(it->path(), entryEc);
		}
		return path;
	}();
	return folder;
}

static std::filesystem::path MakeUniqueJobFilePath()
{
	static std::atomic_uint64_t s_counter{ 0 };
	auto const pid = GetCurrentProcessId();
	auto const ticks = GetTickCount64();
	auto const id = s_counter.fetch_add(1, std::memory_order_relaxed);
	return GetJobFolder() / std::format(L"job_{}_{}_{}.rcj", pid, ticks, id);
}

bool RobocopyJobFile::CanUseInJobFile(std::wstring_view fileName) noexcept
{
	if (fileName.empty())
		return false;

	// robocopy's .rcj line parser:
	// 1. Strips "::" and everything after it as a comment
	// 2. Trims leading spaces/tabs and trailing whitespace (<= 0x20), so filenames with leading/trailing spaces get altered
	// 3. Rejects tokens starting with '-' as switches
	if (auto const initial = fileName.front(); initial == L'-' || initial == L' ' || initial == L'\t')
		return false;
	if (fileName.back() <= L' ')
		return false;
	if (fileName.find(L"::") != std::wstring_view::npos)
		return false;
	// Wildcards in a literal filename would match unintended files
	if (fileName.find_first_of(L"*?") != std::wstring_view::npos)
		return false;

	return true;
}

static void appendUtf8(std::string& dest, std::wstring_view wide)
{
	if (wide.empty())
		return;

	auto const utf8Len = WideCharToMultiByte(
		CP_UTF8,
		0,
		wide.data(),
		static_cast<int>(wide.size()),
		nullptr,
		0,
		nullptr,
		nullptr
	);
	if (utf8Len <= 0)
		return;

	auto const oldSize = dest.size();
	dest.resize(oldSize + utf8Len);
	WideCharToMultiByte(
		CP_UTF8,
		0,
		wide.data(),
		static_cast<int>(wide.size()),
		dest.data() + oldSize,
		utf8Len,
		nullptr,
		nullptr
	);
}

RobocopyJobFile::RobocopyJobFile(std::span<std::wstring const> includeFiles) :
	m_path{ MakeUniqueJobFilePath() }
{
	// RobocopyInjection.dll hooks _wfopen to open .rcj files with "r, ccs=UTF-8".
	// Each UTF-16 code unit takes at most 3 UTF-8 bytes (a surrogate pair is 2 wchar_t -> 4 UTF-8 bytes),
	// plus 3 bytes for '\t' and "\r\n", guaranteeing a single allocation with zero reallocations.
	std::string content = "::\r\n\t/IF\r\n";
	auto const maxBytesNeeded = std::accumulate(
		includeFiles.begin(),
		includeFiles.end(),
		content.size(),
		[](size_t sum, std::wstring const& file) { return sum + file.size() * 3 + 3; }
	);
	content.reserve(maxBytesNeeded);

	for (auto const& file : includeFiles)
	{
		content += '\t';
		appendUtf8(content, file);
		content += "\r\n";
	}

	
	wil::unique_hfile hFile{ CreateFileW(
		m_path.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_TEMPORARY,
		nullptr
	) };
	winrt::check_bool(static_cast<bool>(hFile));

	DWORD written{};
	winrt::check_bool(WriteFile(
		hFile.get(),
		content.data(),
		static_cast<DWORD>(content.size()),
		&written,
		nullptr
	) && written == content.size());
}

RobocopyJobFile::~RobocopyJobFile()
{
	std::error_code ec;
	std::filesystem::remove(m_path, ec);
}

std::wstring_view RobocopyJobFile::PathView() const noexcept
{
	return m_path.native();
}
