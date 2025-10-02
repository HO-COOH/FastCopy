#pragma once
#include <filesystem>
#include <string>
#include <ranges>
#include <array>
#include "TestOperation.h"

class TestCaseBase
{
protected:
	std::filesystem::path m_source;
	std::filesystem::path m_destination;
	TestOperation::Operation m_op = TestOperation::Operation::Copy;
	std::chrono::seconds m_forceSleep{};
public:
	TestCaseBase() = default;
	TestCaseBase(std::filesystem::path source, std::filesystem::path destination);

	TestCaseBase& SetSource(std::filesystem::path source);
	TestCaseBase& SetDestination(std::filesystem::path destination);
	
	std::filesystem::path const& GetSource() const { return m_source; }
	std::filesystem::path const& GetDestination() const { return m_destination; }

	void ClearSource();
	void ClearDestination();

	virtual void Generate() = 0;
	void Restore();
	void SetRestoreAction(TestOperation::Operation op, std::chrono::seconds forceSleep = {}) { m_op = op; m_forceSleep = forceSleep; }

	template<size_t Size = 1024 * 4>
	static auto makeFilledBuffer()
	{
		std::array<char, Size> buffer;
		std::ranges::generate(buffer, [] { return rand() % UCHAR_MAX; });
		return buffer;
	}
};