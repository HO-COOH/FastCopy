#pragma once
#include <vector>
#include <memory>
#include "TestResult.h"
#include "TestOperation.h"

class IImplementationBase;
class TestFactory
{
public:


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
	//static void Clear();

	template<typename TestCase>
	TestFactory& operator<<(TestCase& testCase)
	{
		testCase();
		return *this;
	}
private:

	/**
	 * Verify whether the test run successfully
	 * 
	 * @return true if test runs successfully
	 */
	//static bool verify();
	static std::vector<TestResult> s_testResults;

	/**
	 * Delete previously copied files in the destinations
	 * 
	 */
	//static void clearTestination();
};

