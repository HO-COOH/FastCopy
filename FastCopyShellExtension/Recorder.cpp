#include "Recorder.h"
#include <format>
#include <ShlObj_core.h>
#include <Windows.h>
#include <filesystem>
#include "ShellItem.h"
#include "Registry.h"
#include "AppFolders.h"

Recorder::Recorder(CopyOperation op) : m_path{ GetRecordFilePath(op) }
{
	if (!m_file.Good())
		throw std::runtime_error{ "Cannot open file" };

	// Pointed at only once the file is known to be open, so a record that could not be
	// created leaves the previous one in place instead of a name nothing can read back.
	Registry::Record(m_path.wstring());
}

Recorder& Recorder::operator<<(ShellItem& item)
{
	m_file << item.GetDisplayName();
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

std::filesystem::path Recorder::GetRecordFilePath(CopyOperation op)
{
	return AppFolders::RecordsFolder(true) / MakeRecordFileName(toFlag(op), L".txt");
}

bool Recorder::HasRecord()
{
	std::error_code ec;
	auto const& dir = AppFolders::RecordsFolder();
	if (!std::filesystem::exists(dir, ec) || ec)
		return false;

	return std::find_if(std::filesystem::directory_iterator{ dir }, std::filesystem::directory_iterator{}, [](std::filesystem::directory_entry const& fileEntry) {
		auto str = fileEntry.path().filename().wstring();
		OutputDebugString(std::format(L"{}\n", fileEntry.path().wstring()).data());
		return str.starts_with(L'C') || str.starts_with(L"M2") || str.starts_with(L'P');
	}) != std::filesystem::directory_iterator{};
}
