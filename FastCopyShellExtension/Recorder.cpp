#include "Recorder.h"
#include <format>
#include <chrono>
#include <ShlObj_core.h>
#include <Windows.h>
#include <filesystem>
#include "ShellItem.h"
#include "Registry.h"

static auto GetLocalDataFolder()
{
	static std::wstring ret = []
	{
		wchar_t* ptr;
		SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &ptr);
		std::wstring result{ ptr };
		CoTaskMemFree(ptr);
		auto ret = result + LR"(\packages\FastCopy_tp1tpcm9wdwpy\LocalCache\Local)";

		return ret;
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
	auto const filename = GetRecordFilePath(op);
	Registry{}.write(L"record", filename);
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

std::wstring Recorder::GetRecordFilePath(CopyOperation op)
{
	auto ret = std::format(L"{}\\{}{}.txt", GetLocalDataFolder(), toFlag(op), GetTimeString());
	//MessageBoxW(NULL, ret.data(), L"", 0);
	return ret;
}

bool Recorder::HasRecord()
{
	return !(std::filesystem::is_empty(GetLocalDataFolder()) ||
		std::filesystem::directory_iterator{ GetLocalDataFolder() }->path().filename().string()[0] == 'D');
}
