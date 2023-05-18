#include "pch.h"
#include "CommandLine.h"
#include <ranges>
#include <ShlObj_core.h>
#include <filesystem>
#pragma comment(lib, "Shell32.lib")

static std::vector<std::wstring> const& GetCommandArgs()
{
    static auto const ret = []
    {
        auto const cmd = GetCommandLine();
        int argc{};
        auto argv = CommandLineToArgvW(cmd, &argc);
        auto ret = std::vector<std::wstring>(argv, argv + argc);
        if (ret.size() > 1 && ret[1].back() == L'/')
        {
            ret[1] = ret[1].substr(0, ret[1].find_last_not_of(L"/\\\"") + 1);
        }
        return ret;
    }();
    return ret;
}

Command& Command::Get()
{
    static Command s_instance;
    return s_instance;
}

int Command::Size() const
{
    return GetCommandArgs().size();
}

winrt::hstring Command::GetDestination()
{
    auto& args = GetCommandArgs();
    constexpr static std::wstring_view protocol = L"fastcopy://";
    
    return args.size() == 1 ? winrt::hstring{
#ifdef _DEBUG
        LR"(E:\test\Intel Unison)"
#endif
    } : winrt::hstring{ args[1].substr(protocol.size(), args[1].find(L"|") - protocol.size()) };

}

static auto GetLocalDataFolder()
{
    static auto ret = winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path() + L"\\Local";
    std::filesystem::remove_all((ret + L"\\Microsoft").data());
    return ret;
}

std::wstring Command::RecordFile()
{
    auto& args = GetCommandArgs();
    if (args.size() == 1)
    {
        return std::filesystem::is_empty(GetLocalDataFolder().data()) ?
            L"" :
            std::filesystem::directory_iterator{ GetLocalDataFolder().data() }->path().wstring();
    }
    else
    {
        auto result = args[1].substr(args[1].find(L"|") + 1);
        for (int i = 2; i < args.size(); ++i)
        {
            result += L" ";
            result += args[i];
        }
        return result.substr(0, result.find_last_not_of(L"/\\ ") + 1);
    }
}
