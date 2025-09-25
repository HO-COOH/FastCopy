#pragma once

class ShellItem;
struct IShellItemArray;

class ShellItemArray
{
	IShellItemArray* m_ptr;
public:

	class Iterator
	{
		IShellItemArray* m_ptr{};
		size_t m_index{};
	public:
		Iterator() = default;
		Iterator(IShellItemArray* ptr, size_t index = 0) : m_ptr{ ptr }, m_index{ index } {}

		ShellItem operator*();
		Iterator& operator++();
		bool operator==(Iterator const& rhs) const;

		friend class ShellItemArray;
	};
	ShellItemArray(IShellItemArray* ptr) : m_ptr{ ptr } {}

	/*iterators*/
	Iterator begin();
	Iterator end();

	/*element access*/
	ShellItem front();
	ShellItem back();
	ShellItem operator[](size_t index);

	/*observers*/
	size_t size();
};

