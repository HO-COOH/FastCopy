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
        return std::vector<std::wstring_view>(argv, argv + argc);
    }();
    return ret;
}

Command& Command::Get()
{
    static Command s_instance;
    return s_instance;
}

CopyOperation Command::GetOperation()
{
    return CopyOperation();
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
