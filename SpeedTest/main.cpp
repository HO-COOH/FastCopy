#include "TestFactory.h"
#include "Process.h"
#include "TestCases.h"
#include "Env.h"

#ifdef _DEBUG
#include "COMApiTest.h"
#include "XCopyTest.h"
#endif
/*
	This is a test program for testing various ways of copying/moving files and folders in windows.
	Some of the ways includes:
		- Built-in xcopy.exe 
		- Built-in robocopy.exe
		- Windows Explorer
		- C++'s filesystem library
		- Win32 API
	The results of this program will ultimately affect what methods to be included in the FastCopy's main program.

	To write your implementation of such tests, create a new class and inherits from <ICopyBase> and AutoRegister<Self>
	For an example, look into ---> XCopyTest.h

	Test files and folders and generated in TestFactory::MakeTestPaths()
*/

struct ExplorerGuard
{
	ExplorerGuard()
	{
		puts("Killing explorer.exe for accurate result. It will be restarted after test.\n");
		std::system("taskkill /f /im explorer.exe");
	}

	~ExplorerGuard()
	{
		Process<wchar_t> explorer{ Env::GetFolderPath(Env::SpecialFolder::Windows) + L"\\explorer.exe" };
	}
};

int main()
{
	ExplorerGuard guard;
#ifdef _DEBUG
	//manually add debugging test implementation here...
	//In release build, all tests registered with AutoRegister<Self> runs
	
	//TestFactory::Register(std::make_unique<COMApiTest>());
	TestFactory::Register(std::make_unique<XCopyTest>());
#endif
	TestFactory{} 
		<< Random4KFiles{}
		<< BigFile{};
	TestFactory::RunAllTest();
	TestFactory::PrintResult();
	TestFactory::Clear();
}