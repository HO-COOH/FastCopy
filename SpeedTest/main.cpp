#include "TestFactory.h"
#include <iostream>
#include "Process.h"
#include "Env.h"
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

static void KillExplorer()
{
	std::cout << "Killing explorer.exe for accurate result.\n";
	std::system("taskkill /f /im explorer.exe");
}

static void RestartExplorer()
{
	Process<wchar_t> explorer{ Env::GetFolderPath(Env::SpecialFolder::Windows) + L"\\explorer.exe" };
}

int main()
{
	KillExplorer();
	TestFactory::RunAllTest();
	TestFactory::PrintResult();
	TestFactory::Clear();
	RestartExplorer();
}