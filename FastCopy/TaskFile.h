#pragma once
#include <vector>
#include <string>
#include <mutex>
#include "../FastCopyShellExtension/CopyOperation.h"
class TaskFile
{
public:
	template<typename Iterator>
	class TaskFileIterator
	{
		Iterator m_iter;
		mutable std::mutex m_lock;
	public:
		TaskFileIterator() = default;
		TaskFileIterator(Iterator iter) : m_iter{ iter } {}
		TaskFileIterator& operator++()
		{
			std::lock_guard lock{ m_lock };
			++m_iter;
			return *this;
		}

		auto operator<=>(Iterator rhs) const
		{
			std::lock_guard lock{ m_lock };
			return m_iter <=> rhs;
		}

		bool operator==(Iterator rhs) const
		{
			std::lock_guard lock{ m_lock };
			return m_iter == rhs;
		}

		auto operator<=>(TaskFileIterator<Iterator> const& rhs) const
		{
			std::lock_guard lock{ m_lock };
			return m_iter <=> rhs.m_iter;
		}

		bool operator==(TaskFileIterator<Iterator> const& rhs) const
		{
			std::lock_guard lock{ m_lock };
			return m_iter == rhs.m_iter;
		}

		auto& operator*()
		{
			std::lock_guard lock{ m_lock };
			return *m_iter;
		}

		auto& operator->()
		{
			std::lock_guard lock{ m_lock };
			return &(*m_iter);
		}

		auto get() const
		{
			std::lock_guard lock{ m_lock };
			return m_iter;
		}

		operator Iterator() const
		{
			return get();
		}

		TaskFileIterator& operator=(Iterator rhs)
		{
			std::lock_guard lock{ m_lock };
			m_iter = rhs;
			return *this;
		}
	};

	TaskFile(winrt::hstring const& path) : m_path{ path } {}

	wchar_t const* data() const { return m_path.data(); }
	[[maybe_unused]]int32_t GetNumFiles();
	int32_t GetNumFiles(int index);

	uint64_t GetTotalSize();

	auto begin() { return lines.begin(); }
	auto end() { return lines.end(); }

	int IndexOf(TaskFileIterator<typename std::vector<std::wstring>::iterator> const& iter);

	CopyOperation GetOperation() const;

	winrt::hstring GetPath() const { return m_path; }

	static uint64_t GetSizeOfPath(std::wstring_view path);
private:
	winrt::hstring m_path;
	std::vector<std::wstring> lines;
	std::vector<int> numFiles;
	int totalFiles{};
};

