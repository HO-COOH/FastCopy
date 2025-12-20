#pragma once
#include <iterator>
#include <cstddef>

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
		using difference_type = std::ptrdiff_t;
		using value_type = ShellItem;
		using iterator_concept = std::input_iterator_tag;

		Iterator() = default;
		Iterator(IShellItemArray* ptr, size_t index = 0) : m_ptr{ ptr }, m_index{ index } {}

		ShellItem operator*() const;
		Iterator& operator++();
		Iterator operator++(int);
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

