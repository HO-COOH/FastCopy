#include "Recorder.h"
#include <format>
#include <chrono>
#include <ShlObj_core.h>
#include <Windows.h>
#include <filesystem>
#include "ShellItem.h"
#include "Registry.h"
#include <wil/com.h>
#include <wil/win32_helpers.h>
static auto GetLocalDataFolder()
{
	static std::filesystem::path ret = []
	{
		wil::unique_cotaskmem_string localAppData;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, localAppData.put());
		return std::filesystem::path{ localAppData.get() } / LR"(packages\FastCopy_tp1tpcm9wdwpy\LocalCache\Local)";
	}();
	return ret;
}

static auto GetTimeString()
{
	//std::chrono::current_zone() gives exceptions on Windows 10, 17763 with MSVC cl.exe version 19.36.32535
	//auto ret = std::format(L"{}", std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time());
	auto ret = std::format(L"{}", std::chrono::system_clock::now());
	std::ranges::replace_if(ret, [](auto c) {return c == L'.' || c == L':'; }, L'-');
	return ret;
}

Recorder::Recorder(CopyOperation op) 
{
	std::filesystem::create_directory(GetLocalDataFolder());
	auto const filename = GetRecordFilePath(op).wstring();
	Registry::Record(filename);
	m_fs = _wfopen(filename.data(), L"wb");
	if (!m_fs)
		throw std::runtime_error{ "Cannot open file" };
}

Recorder& Recorder::operator<<(ShellItem& item)
{
	std::wstring buf {item.GetDisplayName()};
	std::transform(buf.begin(), buf.end(), buf.begin(), [](wchar_t c) { return c == L'\\' ? L'/' : c; });
	size_t const length = buf.size();
	fwrite(&length, sizeof(length), 1, m_fs);
	fwrite(buf.data(), 2, length, m_fs);

	return *this;
}

Recorder::~Recorder()
{
	if (m_fs)
		fclose(m_fs);
}

static wchar_t toFlag(CopyOperation op)
{
	switch (op)
	{
		case CopyOperation::Copy:
			return L'C';
		case CopyOperation::Move:
			return L'M';
		case CopyOperation::Paste:
			return L'P';
		case CopyOperation::Delete:
			return L'D';
	}
}

std::filesystem::path Recorder::GetRecordFilePath(CopyOperation op)
{
	return 	GetLocalDataFolder() / std::format(L"{}{}.txt", toFlag(op), GetTimeString());
}

bool Recorder::HasRecord()
{
	return std::find_if(std::filesystem::directory_iterator{ GetLocalDataFolder() }, std::filesystem::directory_iterator{}, [](std::filesystem::directory_entry const& fileEntry) {
		auto str = fileEntry.path().filename().wstring();
		return str.starts_with(L'C') || str.starts_with(L"M2") || str.starts_with(L'P');
	}) != std::filesystem::directory_iterator{};
}
