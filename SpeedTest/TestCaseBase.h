#pragma once
#include <filesystem>
#include <string>
#include <ranges>
#include <array>

class TestCaseBase
{
protected:
	std::filesystem::path m_source;
	std::filesystem::path m_destination;
public:
	TestCaseBase() = default;
	TestCaseBase(std::filesystem::path source, std::filesystem::path destination);

	TestCaseBase& SetSource(std::filesystem::path source);

	TestCaseBase& SetDestination(std::filesystem::path destination);

	void ClearSource();
	void ClearDestination();

	template<size_t Size = 1024 * 4>
	static auto makeFilledBuffer()
	{
		std::array<char, Size> buffer;
		std::ranges::generate(buffer, [] { return rand() % UCHAR_MAX; });
		return buffer;
	}
};