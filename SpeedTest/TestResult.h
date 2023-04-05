#pragma once
#include <string>
#include <chrono>

struct TestResult
{
	std::string m_testName;
	std::chrono::nanoseconds m_duration;

	auto operator<=>(TestResult const& rhs) const
	{
		return m_duration <=> rhs.m_duration;
	}
	auto operator==(TestResult const& rhs) const
	{
		return m_duration == rhs.m_duration;
	}
};