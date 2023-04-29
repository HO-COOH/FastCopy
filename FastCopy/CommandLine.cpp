#include "pch.h"
#include "CommandLine.h"
#include <ranges>
#include <ShlObj_core.h>

std::vector<std::wstring_view> const& GetCommandArgs()
{
    static auto const ret = []
    {
        auto const cmd = GetCommandLine();
        int argc{};
        auto argv = CommandLineToArgvW(cmd, &argc);
        std::wstring result;
        for (auto i = 0; i < argc; ++i)
        {
            result += argv[i];
            result += L'\n';
        }
        MessageBox(NULL, result.data(), nullptr, 0);
        return std::vector<std::wstring_view>(argv, argv + argc);
    }();
    return ret;
}

Command& Command::Get()
{
    static Command s_instance;
    return s_instance;
}

winrt::hstring Command::GetDestination()
{
    auto& args = GetCommandArgs();
    constexpr static std::wstring_view protocol = L"fastcopy://";
    return args.size() == 1 ? winrt::hstring{ LR"(E:\test)" } :  winrt::hstring{ args[1].substr(protocol.size()) };
}

static auto GetLocalDataFolder()
{
    //static std::wstring ret = []
    //{
    //    wchar_t* ptr;
    //    SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &ptr);
    //    std::wstring result{ ptr };
    //    CoTaskMemFree(ptr);
    //    return result;
    //}();
    //return ret;

    return winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path() + L"\\Local";
}
#include <filesystem>
#pragma comment(lib, "Shell32.lib")
std::wstring Command::RecordFile()
{
    if (std::filesystem::is_empty(GetLocalDataFolder().data()))
        return L"";
    
    return std::filesystem::directory_iterator{ GetLocalDataFolder().data()}->path().wstring();
}
