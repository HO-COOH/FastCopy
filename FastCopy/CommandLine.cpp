#include "pch.h"
#include "CommandLine.h"
#include <ranges>
#include <ShlObj_core.h>
#include <filesystem>

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

winrt::hstring Command::GetDestination()
{
    auto& args = GetCommandArgs();
    constexpr static std::wstring_view protocol = L"fastcopy://";
    
    return args.size() == 1 ? winrt::hstring{ 
#ifdef _DEBUG
        LR"(E:\test\Intel Unison/)" 
#endif
    } :  winrt::hstring{ args[1].substr(protocol.size()) };

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
    static auto ret = winrt::Windows::Storage::ApplicationData::Current().LocalCacheFolder().Path() + L"\\Local";
    std::filesystem::remove_all((ret + L"\\Microsoft").data());
    return ret;
}
#include <filesystem>
#pragma comment(lib, "Shell32.lib")
std::wstring Command::RecordFile()
{
    if (std::filesystem::is_empty(GetLocalDataFolder().data()))
        return L"";
    
    return std::filesystem::directory_iterator{ GetLocalDataFolder().data()}->path().wstring();
}
