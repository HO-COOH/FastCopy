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

	/**
	 * Run by the AutoRegister or custom register test cases
	 * 
	 * @param test A pointer to the test class
	 */
	static void Register(std::unique_ptr<ITestBase>&& test);

	/**
	 * Called to generate test folders and files
	 * 
	 */
	static void MakeTestPaths();

	/**
	 * Print sorted test results, eg: 
	 *	#0 <TestName> ==> <seconds>
	 * 
	 */
	static void PrintResult();

	/**
	 * Clear test files and folders.
	 * 
	 */
	static void Clear();
private:

	/**
	 * Verify whether the test run successfully
	 * 
	 * @return true if test runs successfully
	 */
	static bool verify();
	static std::vector<TestOperation> s_paths;
	static std::vector<std::unique_ptr<ITestBase>> s_tests;
	static std::vector<TestResult> s_testResults;

	/**
	 * Delete previously copied files in the destinations
	 * 
	 */
	static void clearTestination();
	constexpr static auto randomFileTotalBytes = 1024ull * 1024 * 1024;
};

