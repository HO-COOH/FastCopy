#include "TestFactory.h"
#include "TestResult.h"
#include <ranges>
#include "ITestBase.h"
#include "Env.h"
#include <fstream>
#include <format>
#include <iostream>
#include <filesystem>

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
		clearTestination();
	}
	std::ranges::sort(s_testResults);
}

void TestFactory::Register(std::unique_ptr<ITestBase>&& test)
{
	s_tests.push_back(std::move(test));
}

static void createFile(std::wstring_view path, char const* buffer)
{
	std::ofstream fs{ path.data() , std::ios::binary};
	fs.write(buffer, 4 * 1024);
}
void TestFactory::MakeTestPaths()
{
	auto const before = std::chrono::steady_clock::now();
	size_t totalBytes{};
	char buffer[1024 * 4]{};
	auto folder = Env::GetFolderPath(Env::SpecialFolder::Desktop) + Env::GetRandomName();
	std::ranges::generate(buffer, [] {return rand() % 255; });

	while (totalBytes < randomFileTotalBytes) //1GB of 4K random files
	{
		createFile(folder + L"\\" + std::to_wstring(totalBytes), buffer);
		totalBytes += std::size(buffer);
	}
	s_paths.push_back(
		TestOperation{
			folder,
			Env::GetTestDestinationPath(Env::GetRandomName(), randomFileTotalBytes)
		}
	);
	puts(std::format(
		"Make test files ==> {} seconds\n",
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - before)
	).data());
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
