#include "TestFactory.h"

/*
	This is a test program for testing various ways of copying/moving files and folders in windows.
	Some of the ways includes:
		- Built-in xcopy.exe 
		- Built-in robocopy.exe
		- Windows Explorer
		- C++'s filesystem library
		- Win32 API
	The results of this program will ultimately affect what methods to be included in the FastCopy's main program.

	To write your implementation of such tests, create a new class and inherits from <ITestBase> and AutoRegister<Self>
	For an example, look into ---> XCopyTest.h

	Test files and folders and generated in TestFactory::MakeTestPaths()
*/
int main()
{
	TestFactory::RunAllTest();
	TestFactory::PrintResult();
}