#include "pch.h"
#include "CommandLine.h"
#include <ranges>
#include <ShlObj_core.h>
#include <filesystem>
#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")


void Command::Set(PWSTR cmd)
{
    int argc{};
    auto argv = CommandLineToArgvW(cmd, &argc);
    if (std::wstring_view{ cmd }.starts_with(protocol))
    {
        m_args.push_back(std::wstring{ cmd });
        return;
    }

    std::copy(argv, argv + argc, std::back_inserter(m_args));
    if (m_args.size() > 1 && m_args[1].back() == L'/')
    {
        m_args[1] = m_args[1].substr(0, m_args[1].find_last_not_of(L"/\\\"") + 1);
    }
}

Command& Command::Get()
{
    static Command s_instance;
    return s_instance;
}

winrt::hstring Command::GetDestination()
{
    auto destination = m_args[0].substr(protocol.size(), m_args[0].find(L"|") - protocol.size());
    bool const isQuoted = destination.starts_with(L"\"");
    assert(isQuoted && destination.ends_with(L"\""));
    return winrt::hstring{ isQuoted ? destination.substr(1, destination.size() - 2) : std::move(destination) };
}

static auto GetLocalDataFolder()
{
    static auto ret = winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path() + L"\\Local";
    std::filesystem::remove_all((ret + L"\\Microsoft").data());
    return ret;
}

std::wstring Command::RecordFile()
{
    auto result = m_args[0].substr(m_args[0].find(L"|") + 2);
    for (int i = 1; i < m_args.size(); ++i)
    {
        result += L" ";
        result += m_args[i];
    }
    return result.substr(0, result.find_last_not_of(L"/\\ \"") + 1);

}
