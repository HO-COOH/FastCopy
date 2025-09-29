#pragma once
#include <filesystem>
#include <string>

class TestCaseBase
{
	std::filesystem::path m_source;
	std::filesystem::path m_destination;
public:
	TestCaseBase() = default;
	TestCaseBase(std::filesystem::path source, std::filesystem::path destination);
	TestCaseBase(std::wstring source, std::wstring destination);

	TestCaseBase& SetSource(std::filesystem::path source);
	TestCaseBase& SetSource(std::wstring source);

	TestCaseBase& SetDestination(std::filesystem::path destination);
	TestCaseBase& SetDestination(std::wstring destination);

	void ClearSource();
	void ClearDestination();
};