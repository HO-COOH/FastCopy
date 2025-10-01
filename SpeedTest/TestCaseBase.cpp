#include "TestCaseBase.h"

TestCaseBase::TestCaseBase(std::filesystem::path source, std::filesystem::path destination) : 
	m_source{std::move(source)},
	m_destination{std::move(destination)}
{
	ClearDestination();
}

TestCaseBase& TestCaseBase::SetSource(std::filesystem::path source)
{
	m_source = std::move(source);
	return *this;
}

TestCaseBase& TestCaseBase::SetDestination(std::filesystem::path destination)
{
	m_destination = std::move(destination);
	return *this;
}

void TestCaseBase::ClearSource()
{
	std::filesystem::remove_all(m_source);
}

void TestCaseBase::ClearDestination()
{
	std::filesystem::remove_all(m_destination);
	std::filesystem::create_directories(m_destination);
}
