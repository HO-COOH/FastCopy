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
                    std::format(LR"("{}" "{}" /E /NC /NS /R:0 /W:0 /MT:64)", item.source, item.destination) :
                    std::format(LR"("{}" "{}" /E /NC /NS /R:0 /W:0 /MT:64 /MOVE)", item.source, item.destination)
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
