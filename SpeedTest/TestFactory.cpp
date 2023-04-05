#include "TestFactory.h"
#include "TestResult.h"
#include <ranges>
#include "ITestBase.h"
#include "Env.h"

std::vector<TestOperation> TestFactory::s_paths;
std::vector<std::unique_ptr<ITestBase>> TestFactory::s_tests;
std::vector<TestResult> TestFactory::s_testResults;

void TestFactory::RunAllTest()
{
	static bool isFirstRun = true;
	if (isFirstRun)
	{
		MakeTestPaths();
		isFirstRun = false;
	}
	for (auto& test : s_tests)
	{
		auto before = std::chrono::steady_clock::now();
		if (test->Run(s_paths) && verify())
			s_testResults.push_back(TestResult{ test->GetName(), std::chrono::steady_clock::now() - before });
	}
	std::ranges::sort(s_testResults);
}

void TestFactory::Register(std::unique_ptr<ITestBase>&& test)
{
	s_tests.push_back(std::move(test));
}

void TestFactory::MakeTestPaths()
{
	s_paths.push_back(
		TestOperation{
			Env::GetFolderPath(Env::SpecialFolder::Desktop),
			LR"(E:\)"
		}
	);
}

void TestFactory::PrintResult()
{
}

bool TestFactory::verify()
{
	return true;
}
