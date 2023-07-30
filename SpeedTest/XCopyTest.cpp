#include "XCopyTest.h"
#include "Process.h"
#include "Env.h"
#include "Config.h"

std::wstring const XCopyTest::Application = Env::GetFolderPath(Env::SpecialFolder::System32) + LR"(\xcopy.exe)";
bool XCopyTest::Run(std::vector<TestOperation> const& paths)
{
	std::vector<Process<wchar_t>> processes;
	for (auto const& item : paths)
	{
		processes.push_back(Process<wchar_t>{
			XCopyTest::Application,
			std::format(LR"("{}" "{}" /Y /E /C /I /H)", convert_to_wstring(item.source), convert_to_wstring(item.destination))
		});
	}
	WaitForAllProcesses<wchar_t>(processes);
	return true;
}

std::string XCopyTest::GetName() const
{
	return "XCopy";
}
