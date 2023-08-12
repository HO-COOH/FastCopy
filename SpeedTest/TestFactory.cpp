#include "TestFactory.h"
#include "TestResult.h"
#include <ranges>
#include "ICopyBase.h"
#include "Env.h"
#include <filesystem>
#include <format>
#include <iostream>
#include "Config.h"

std::vector<TestResult> TestFactory::s_testResults;

//void TestFactory::RunAllTest()
//{
//	for (auto& test : s_implementations)
//	{
//		auto name = test->GetName();
//		std::cout << "Testing: " << name;
//		test->started();
//		auto before = std::chrono::steady_clock::now();
//		if (test->Run(s_paths) && verify())
//		{
//			std::cout << "\t Success.";
//			s_testResults.push_back(TestResult{ test->GetName(), std::chrono::steady_clock::now() - before });
//		}
//		else
//		{
//			std::cout << "\t Failed.";
//		}
//		test->finished();
//		std::cout << "\t Clearing ";
//		//clearTestination();
//		std::cout << "finished.\n";
//	}
//	std::ranges::sort(s_testResults);
//}

void TestFactory::PrintResult()
{
	for (int i = 0; i < std::size(s_testResults); ++i)
	{
		Env::Puts(std::format(
			"#{}: {} ==> {} seconds\n",
			i,
			s_testResults[i].m_testName,
			std::chrono::duration_cast<std::chrono::seconds>(s_testResults[i].m_duration)
		).data());
	}
}

//void TestFactory::Clear()
//{
//	for (auto const& path : s_paths)
//		std::filesystem::remove_all(path.source);
//}
//
//bool TestFactory::verify()
//{
//	return true;
//}
//
//void TestFactory::clearTestination()
//{
//	for (auto const& path : s_paths)
//		std::filesystem::remove_all(path.destination);
//}
