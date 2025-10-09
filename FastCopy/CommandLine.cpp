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
    m_args.reserve(argc);
    auto argv = CommandLineToArgvW(cmd, &argc);
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
    constexpr static std::wstring_view protocol = L"fastcopy://";
    
    return winrt::hstring{ m_args[0].substr(protocol.size(), m_args[0].find(L"|") - protocol.size()) };

}

static auto GetLocalDataFolder()
{
    static auto ret = winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path() + L"\\Local";
    std::filesystem::remove_all((ret + L"\\Microsoft").data());
    return ret;
}

std::wstring Command::RecordFile()
{
    if (m_args.size() == 1)
    {
        return std::filesystem::is_empty(GetLocalDataFolder().data()) ?
            L"" :
            std::filesystem::directory_iterator{ GetLocalDataFolder().data() }->path().wstring();
    }
    else
    {
        auto result = m_args[0].substr(m_args[0].find(L"|") + 2);
        for (int i = 1; i < m_args.size(); ++i)
        {
            result += L" ";
            result += m_args[i];
        }
        return result.substr(0, result.find_last_not_of(L"/\\ ") + 1);
    }
}
