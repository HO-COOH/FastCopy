#pragma once
#include <vector>
#include <memory>
#include "TestResult.h"
#include "TestOperation.h"

class ITestBase;
class TestFactory
{
public:
	static void RunAllTest();

	static void Register(std::unique_ptr<ITestBase>&& test);

	static void MakeTestPaths();

	static void PrintResult();
private:
	static bool verify();
	static std::vector<TestOperation> s_paths;
	static std::vector<std::unique_ptr<ITestBase>> s_tests;
	static std::vector<TestResult> s_testResults;
};

