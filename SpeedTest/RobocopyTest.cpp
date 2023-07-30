#include "RobocopyTest.h"
#include <vector>
#include "Process.h"
#include "Env.h"
#include "Config.h"

std::wstring const RobocopyTest::ApplicationName = Env::GetFolderPath(Env::SpecialFolder::System32) + L"\\robocopy.exe";
bool RobocopyTest::Run(std::vector<TestOperation> const& paths)
{
    std::vector<Process<wchar_t>> processes;
    for (auto const& item : paths)
    {
        processes.push_back(
            Process<wchar_t>{
                ApplicationName,
                item.operation == TestOperation::Operation::Copy?
                    std::format(LR"("{}" "{}" /E /MT:64)", convert_to_wstring(item.source), convert_to_wstring(item.destination)) :
                    std::format(LR"("{}" "{}" /E /MT:64 /MOVE)", convert_to_wstring(item.source), convert_to_wstring(item.destination))
            }
        );
    }
    WaitForAllProcesses<wchar_t>(processes);
    return true;
}

std::string RobocopyTest::GetName() const
{
    return "Robocopy";
}
