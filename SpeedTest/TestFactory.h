#pragma once
#include <vector>
#include <memory>
#include "TestResult.h"
#include "TestOperation.h"

class ICopyBase;
class TestFactory
{
public:

	static void RunAllTest();

	/**
	 * Run by the AutoRegister or custom register test cases
	 * 
	 * @param test A pointer to the test class
	 */
	static void Register(std::unique_ptr<ICopyBase>&& test);

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

	template<typename TestCase>
	TestFactory& operator<<(TestCase const& testCase)
	{
		testCase(s_paths);
		return *this;
	}
private:

	/**
	 * Verify whether the test run successfully
	 * 
	 * @return true if test runs successfully
	 */
	static bool verify();
	static std::vector<TestOperation> s_paths;
	static std::vector<std::unique_ptr<ICopyBase>> s_implementations;
	static std::vector<TestResult> s_testResults;

	/**
	 * Delete previously copied files in the destinations
	 * 
	 */
	static void clearTestination();
};

