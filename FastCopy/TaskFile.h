#pragma once
#include <vector>
#include <string>
class TaskFile
{
public:
	using TaskFileIterator = std::vector<std::wstring>::iterator;

	TaskFile(winrt::hstring const& path) : m_path{ path } {}

	wchar_t const* data() const { return m_path.data(); }
	[[maybe_unused]]int32_t GetNumFiles();
	int32_t GetNumFiles(int index);

	auto begin() { return lines.begin(); }
	auto end() { return lines.end(); }

	int IndexOf(TaskFileIterator const& iter);
private:
	winrt::hstring m_path;
	std::vector<std::wstring> lines;
	std::vector<std::optional<int>> numFiles;
	int totalFiles{};
};

