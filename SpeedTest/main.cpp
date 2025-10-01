#define _SILENCE_CXX20_CISO646_REMOVED_WARNING //needed for C++20 with Absl
#include "TestFactory.h"
#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include "TUI.h"
#include "COMInitializeHelper.h"
#include "BasicInteraction.h"
#include <iostream>

/*
	The following flags are for configuring what test cases to be run via command lines.
	For example, there might be cases when you want to generate random 4K files with this tool,
	and then test the performance of Windows' built-in File Explorer.

	If no flags are specified, this program should run everything, including generating test files,
	and run all the implemented copy methods.
*/
ABSL_FLAG(bool, generate_random_files, false, "Generate 1GB of random 4K files");
ABSL_FLAG(bool, generate_big_files, false, "Generate 4GB of big files");


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



/**
 * If there is command line args, run what command line specified.
 */
static void RunCommandLineConfig(...)
{
	//if (absl::GetFlag(FLAGS_generate_random_files))
	//	TestFactory{} << Random4KFiles{};
	//if (absl::GetFlag(FLAGS_generate_big_files))
	//	TestFactory{} << BigFile{};
}


//1.Generate 4k files (1GB / 4KB = 262,144 )
//2.Generate big files (4GB / 1GB = 4)
//3.Generate mixed files (5GB total)
//4.Run copy
//5.Run move
//6.Run delete

int main(int argc, char** argv)
{
	try 
	{
		if (argc == 1)
		{
			BasicInteraction::Run();
			//Config::GetInstance().m_printConsole = false;
			//TUI::Run();
		}
		else
		{
			RunCommandLineConfig(absl::ParseCommandLine(argc, argv));
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << '\n';
	}
}