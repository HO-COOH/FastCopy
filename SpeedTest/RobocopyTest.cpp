#include "RobocopyTest.h"
#include <vector>
#include "Process.h"
#include "Env.h"
#include "Config.h"
#include <iostream>
#include <thread>

std::wstring const RobocopyTest::ApplicationName = Env::GetFolderPath(Env::SpecialFolder::System32) + L"\\robocopy.exe";
bool RobocopyTest::Run(std::vector<TestOperation> const& paths)
{
    std::vector<Process<wchar_t>> processes;
    for (auto const& item : paths)
    {
        auto const threadCount = std::thread::hardware_concurrency();
        auto cmd = item.operation == TestOperation::Operation::Copy ?
            std::format(LR"("{}" "{}" /J /E /NC /NS /R:0 /W:0 /MT:{})", item.source, item.destination, threadCount) :
            std::format(LR"("{}" "{}" /J /E /NC /NS /R:0 /W:0 /MT:{} /MOVE)", item.source, item.destination, threadCount);
        std::wcout << L"Execute robocopy -> " << cmd << L'\n';
        processes.push_back(
            Process<wchar_t>
            {
                ApplicationName,
                std::move(cmd)
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
