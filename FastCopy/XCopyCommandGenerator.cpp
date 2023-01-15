#include "pch.h"
#include "XCopyCommandGenerator.h"
#include <format>

static inline auto GetFolderName(std::string_view path)
{
    return path.substr(path.rfind("\\") + 1);
}

std::string XCopyCommandGenerator::makeCommand(Task const& task, winrt::FastCopy::Mode mode)
{
    std::string command =
        task.pItem.Type() == winrt::FastCopy::ExplorerItemType::File ?
        std::format(R"(C:\Windows\system32\xcopy.exe "{}" "{}\" )",
            task.source,
            task.destination) :
        std::format(R"(C:\Windows\system32\xcopy.exe "{}" "{}{}\" /E )",
            task.source,
            task.destination,
            GetFolderName(task.source));

    switch (mode)
    {
        case winrt::FastCopy::Mode::Copy:
        case winrt::FastCopy::Mode::Move:
            command += "/I /Y /H /C";
            break;
        case winrt::FastCopy::Mode::Custom:
            command += appendCustomCommand();
            break;
        default:
            throw std::exception{ "Invalid mode" };
    }
    return command;
}

XCopyCommandGenerator& XCopyCommandGenerator::GetInstance()
{
    static XCopyCommandGenerator s_instance;
    return s_instance;
}

std::string XCopyCommandGenerator::appendCustomCommand()
{
    std::string flags;
    for (auto flag : m_flags)
    {
        auto flagReal = flag.as<winrt::FastCopy::XCopyFlags>();
        if (flagReal.Enabled())
        {
            flags += winrt::to_string(flagReal.Flag());
            flags += " ";
        }
    }

    return flags;
}
