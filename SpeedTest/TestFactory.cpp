#include "TestFactory.h"
#include "TestResult.h"
#include <ranges>
#include "ICopyBase.h"
#include "Env.h"
#include <filesystem>
#include <format>
#include <iostream>
#include "Config.h"

std::vector<TestOperation> TestFactory::s_paths;
std::vector<std::unique_ptr<ICopyBase>> TestFactory::s_implementations;
std::vector<TestResult> TestFactory::s_testResults;

void TestFactory::RunAllTest()
{
	for (auto& test : s_implementations)
	{
		auto name = test->GetName();
		std::cout << "Testing: " << name;
		auto before = std::chrono::steady_clock::now();
		if (test->Run(s_paths) && verify())
		{
			std::cout << "\t Success.";
			s_testResults.push_back(TestResult{ test->GetName(), std::chrono::steady_clock::now() - before });
		}
		else
		{
			std::cout << "\t Failed.";
		}
		std::cout << "\t Clearing ";
		clearTestination();
		std::cout << "finished.\n";
	}
	std::ranges::sort(s_testResults);
}

void TestFactory::Register(std::unique_ptr<ICopyBase>&& test)
{
	s_implementations.push_back(std::move(test));
}

void TestFactory::PrintResult()
{
	for (int i = 0; i < std::size(s_testResults); ++i)
	{
		puts(std::format(
			"#{}: {} ==> {} seconds\n",
			i,
			s_testResults[i].m_testName,
			std::chrono::duration_cast<std::chrono::seconds>(s_testResults[i].m_duration)
		).data());
	}
}

void TestFactory::Clear()
{
	for (auto const& path : s_paths)
		std::filesystem::remove_all(path.source);
}

bool TestFactory::verify()
{
	return true;
}

void TestFactory::clearTestination()
{
	for (auto const& path : s_paths)
		std::filesystem::remove_all(path.destination);
}
