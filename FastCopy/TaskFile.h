#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>
#include "../FastCopyShellExtension/CopyOperation.h"

class TaskFile
{
public:
	struct TaskCount
	{
		int32_t totalFiles{};
		uint64_t totalBytes{};
	};

	struct TaskItem
	{
		std::wstring path;
		uint64_t size{};
		int32_t numFiles{};
		bool isDirectory{};
	};

	TaskFile(winrt::hstring const& path);

	wchar_t const* data() const noexcept { return m_path.data(); }

	//Walks every loaded entry once, computing the item count and the total byte size together.
	//Expensive (recurses into folders); intended to run on a background thread.
	[[nodiscard]] TaskCount ComputeCountAndSize();

	constexpr int32_t GetNumFiles(size_t index) const noexcept { return m_items[index].numFiles; }
	constexpr size_t GetItemCount() const noexcept { return m_items.size(); }
	constexpr std::wstring const& GetItemPath(size_t index) const noexcept { return m_items[index].path; }
	constexpr bool IsDirectory(size_t index) const noexcept { return m_items[index].isDirectory; }
	constexpr uint64_t GetItemSize(size_t index) const noexcept { return m_items[index].size; }
	constexpr CopyOperation GetOperation() const noexcept { return m_operation; }
	constexpr winrt::hstring const& GetPath() const noexcept { return m_path; }
private:
	void parseItems(std::span<std::byte const> buffer);

	winrt::hstring m_path;
	std::vector<TaskItem> m_items;
	CopyOperation m_operation{ CopyOperation::Copy };
	bool m_computed{};
};
