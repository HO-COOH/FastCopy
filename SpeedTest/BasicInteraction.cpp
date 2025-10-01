#include "BasicInteraction.h"
#include <iostream>
#include "Random4KFiles.h"
#include "BigFiles.h"
#include "FileOpenDialog.h"
#include "COMInitializeHelper.h"
#include "XCopyTest.h"
#include "RobocopyTest.h"
#include "FilesystemApiTest.h"
#include "COMApiTest.h"
#include "Win32ApiTest.h"

class ScopeTimer
{
	std::string_view m_name;
	std::chrono::steady_clock::time_point m_current = std::chrono::steady_clock::now();
public:
	ScopeTimer(std::string_view name) : m_name{ name } {}
	~ScopeTimer()
	{
		auto duration = std::chrono::steady_clock::now() - m_current;
		auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
		auto min = std::chrono::duration_cast<std::chrono::minutes>(totalSeconds);
		puts(std::format("{} finished in {}:{:02d}", m_name, min.count(), (totalSeconds - min).count()).data());
	}
};

void BasicInteraction::generateFiles()
{
	for (auto option : {
		"[1]: Generate 4K files",
		"[2]: Generate big files"
		"Select an option: "
		})
		puts(option);

	int selection;
	std::cin >> selection;

	if (selection != 1 && selection != 2)
		return;

	source = FileOpenDialog::PickSingleFolder();
	destination = FileOpenDialog::PickSingleFolder();

	switch (selection)
	{
		case 1:
		{
			Random4KFiles{ 1024 * 1024 * 1024, source.GetDisplayName(), destination.GetDisplayName() }.Generate();
			break;
		}
		case 2:
		{
			BigFile{ 1024 * 1024 * 1024 * 4ull, source.GetDisplayName(), destination.GetDisplayName() }.Generate();
			break;
		}
		default:
			break;
	}
}

void BasicInteraction::runImplementation()
{
	for (auto option : {
		"[1]: Run copy",
		"[2]: Run move",
		"[3]: Run delete"
	})
		puts(option);

	int selection;
	std::cin >> selection;

	if (selection != 1 && selection != 2 && selection != 3)
		return;

	TestOperation op
	{
		.source = source.GetDisplayName(),
		.destination = destination.GetDisplayName(),
		.operation = static_cast<TestOperation::Operation>(selection - 1)
	};

	TestCaseBase base{ source.GetDisplayName(), destination.GetDisplayName() };
	
	if (op.operation != TestOperation::Operation::Delete && op.operation != TestOperation::Operation::Move)
	{
		ScopeTimer t{ "XCopy" };
		XCopyTest xcopy;
		xcopy.Run({ op });
	}
	else
	{
		std::cerr << "XCopy does not support move / delete operation, skipping...\n";
	}
	base.ClearDestination();

	if (op.operation != TestOperation::Operation::Delete)
	{
		ScopeTimer t{ "Robocopy" };
		RobocopyTest robocopy;
		robocopy.Run({ op });
	}
	else
	{
		std::cerr << "Robocopy does not support delete operation, skipping...\n";
	}
	base.ClearDestination();

	{
		ScopeTimer t{ "std::filesystem" };
		FilesystemApiTest stdFileSystem;
		stdFileSystem.Run({ op });
	}

	{
		ScopeTimer t{ "IFileOperation" };
		COMApiTest comApi;
		comApi.Run({ op });
	}

	{
		ScopeTimer t{ "Win32 SHFileOperation" };
		Win32ApiTest win32Api;
		win32Api.Run({ op });
	}
}

void BasicInteraction::Run()
{
	COMInitializeHelper comInit;
	BasicInteraction instance;
	instance.generateFiles();
	instance.runImplementation();
}
