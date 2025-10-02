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
		puts(std::format("===={} finished in {}:{:02d}====", m_name, min.count(), (totalSeconds - min).count()).data());
	}
};

void BasicInteraction::generateFiles()
{
	for (auto option : {
		"[1]: Generate 4K files",
		"[2]: Generate big files",
		"Select an option: "
		})
		puts(option);

	int selection;
	std::cin >> selection;

	if (selection != 1 && selection != 2)
		return;

	auto source = FileOpenDialog::PickSingleFolder(L"Pick a source folder where test files are generated");
	auto destination = FileOpenDialog::PickSingleFolder(L"Pick a destination folder where test files are copied / moved");

	switch (selection)
	{
		case 1:
		{
#if (defined DEBUG) || (defined _DEBUG)
			constexpr static auto RandomFileSize = 1024 * 1024 * 1;
#else
			constexpr static auto RandomFileSize = 1024 * 1024 * 1024;
#endif
			test = std::make_unique<Random4KFiles>(RandomFileSize, source.GetDisplayName(), destination.GetDisplayName());
			test->Generate();
			break;
		}
		case 2:
		{
			test = std::make_unique<BigFile>(1024 * 1024 * 1024 * 4ull, source.GetDisplayName(), destination.GetDisplayName());
			test->Generate();
			m_forceSleep = true;
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
		.source = test->GetSource(),
		.destination = test->GetDestination(),
		.operation = static_cast<TestOperation::Operation>(selection - 1)
	};
	test->SetRestoreAction(op.operation, std::chrono::seconds{ m_forceSleep ? 10 : 0 });
	
	if (op.operation != TestOperation::Operation::Delete && op.operation != TestOperation::Operation::Move)
	{
		{
			ScopeTimer t{ "XCopy" };
			XCopyTest xcopy;
			xcopy.Run({ op });
		}
		test->Restore();
	}
	else
	{
		std::cerr << "XCopy does not support move / delete operation, skipping...\n";
	}

	if (op.operation != TestOperation::Operation::Delete)
	{
		{
			ScopeTimer t{ "Robocopy" };
			RobocopyTest robocopy;
			robocopy.Run({ op });
		}
		test->Restore();
	}
	else
	{
		std::cerr << "Robocopy does not support delete operation, skipping...\n";
	}

	{
		{
			ScopeTimer t{ "std::filesystem" };
			FilesystemApiTest stdFileSystem;
			stdFileSystem.Run({ op });
		}
		test->Restore();
	}

	{
		{
			ScopeTimer t{ "IFileOperation" };
			COMApiTest comApi;
			comApi.Run({ op });
		}
		test->Restore();
	}

	{
		{
			ScopeTimer t{ "Win32 SHFileOperation" };
			Win32ApiTest win32Api;
			win32Api.Run({ op });
		}
		test->Restore();
	}
}

void BasicInteraction::Run()
{
	COMInitializeHelper comInit;
	BasicInteraction instance;
	instance.generateFiles();
	instance.runImplementation();
}
