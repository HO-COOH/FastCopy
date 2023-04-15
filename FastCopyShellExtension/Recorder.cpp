#include "Recorder.h"
#include <format>
#include <chrono>
#include <ShlObj_core.h>
#include <Windows.h>
#include <filesystem>

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
	auto ret = std::format(L"{}", std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time());
	std::ranges::replace_if(ret, [](auto c) {return c == L'.' || c == L':'; }, L'-');
	return ret;
}

Recorder::Recorder(CopyOperation op) 
{
	std::filesystem::remove_all(GetLocalDataFolder());
	std::filesystem::create_directory(GetLocalDataFolder());
	m_fs = std::wofstream{ GetRecordFilePath(op) };
	if (!m_fs.is_open())
		throw std::runtime_error{ "Cannot open file" };
}

Recorder& Recorder::operator<<(IShellItem& item)
{
	wchar_t* name;
	item.GetDisplayName(SIGDN_FILESYSPATH, &name);
	m_fs << name << L'\n';
	return *this;
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
