#include "TestCaseBase.h"
#include <thread>

TestCaseBase::TestCaseBase(std::filesystem::path source, std::filesystem::path destination) : 
	m_source{std::move(source)},
	m_destination{std::move(destination)}
{
	ClearSource();
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
	std::filesystem::create_directories(m_source);
}

void TestCaseBase::ClearDestination()
{
	std::filesystem::remove_all(m_destination);
	std::filesystem::create_directories(m_destination);
}

void TestCaseBase::Restore
()
{
	if (m_forceSleep.count())
	{
		puts("Force sleep before restore for a while...");
		std::this_thread::sleep_for(m_forceSleep);
	}

	switch (m_op)
	{
		case TestOperation::Operation::Copy:
			ClearDestination();
			break;
		case TestOperation::Operation::Move:
			ClearDestination();
			std::filesystem::create_directories(m_source);
			Generate();
			break;
		case TestOperation::Operation::Delete:
			ClearSource();
			ClearDestination();
			Generate();
			break;
		default:
			break;
	}

	if (m_forceSleep.count())
	{
		puts("Force sleep after restore for a while...");
		std::this_thread::sleep_for(m_forceSleep);
	}
}