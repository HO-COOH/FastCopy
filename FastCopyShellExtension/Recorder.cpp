
#include <format>
#include <chrono>
#include <optional>
#include <filesystem>
#include <algorithm>

#include <ShlObj_core.h>
#include <Windows.h>

#include "Recorder.h"
#include "ShellItem.h"
#include "Registry.h"
#include "DebugPrint.h"
#include "CommonSharedSettings.h"

#include <wil/com.h>
#include <wil/win32_helpers.h>

namespace
{
    static std::wstring GetTimeString()
    {
        //std::chrono::current_zone() gives exceptions on Windows 10, 17763 with MSVC cl.exe version 19.36.32535
        //auto ret = std::format(L"{}", std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time());
        auto ret = std::format(L"{}", std::chrono::system_clock::now());
        std::ranges::replace_if(ret, [](auto c) {return c == L'.' || c == L':'; }, L'-');
        return ret;
    }

    static wchar_t toFlag(CopyOperation op)
    {
        switch (op)
        {
        case CopyOperation::Copy:   return L'C';
        case CopyOperation::Move:   return L'M';
        case CopyOperation::Paste:  return L'P';
        case CopyOperation::Delete: return L'D';
        default: break;
        }
        return L'?';
    }
}

Recorder::Recorder(CopyOperation op) 
{
    auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
    auto const& folderOpt = s.GetLocalDataFolder();
    if (!folderOpt)
    {
        FC_LOG_ERROR(L"LocalDataFolder unavailable, skip recording.");
        return;
    }
    
    auto const& folder = *folderOpt;

    std::error_code ec;
    std::filesystem::create_directories(folder, ec);
    if (ec)
    {
        FC_LOG_WARN(L" create_directories('{}') failed, ec={}",
            folder.wstring(), ec.value());
    }

    auto const filePath = Recorder::GetRecordFilePath(op);
    if (filePath.empty())
    {
        FC_LOG_WARN(L" GetRecordFilePath returned empty path, skip.");
        return;
    }

    Registry::Record(filePath.wstring());
    m_fs = _wfopen(filePath.c_str(), L"wb");
    if (!m_fs)
    {
        FC_LOG_ERROR(L" _wfopen('{}') failed.", filePath.wstring());
        return; // skip recording
    }
}

Recorder& Recorder::operator<<(ShellItem& item)
{
    // If recorder file is not available, skip recording.
    // Getting LocalDataFolder can fail and result in an empty path
    // , causing subsequent file opening operations to fail as well.
    if (!m_fs)
        return *this;

    std::wstring buf {item.GetDisplayName()};
    std::transform(buf.begin(), buf.end(), buf.begin(), [](wchar_t c) { return c == L'\\' ? L'/' : c; });
    size_t const length = buf.size();
    fwrite(&length, sizeof(length), 1, m_fs);
    fwrite(buf.data(), 2, length, m_fs);

    return *this;
}

Recorder::~Recorder()
{
    if (m_fs)
        fclose(m_fs);
}

std::filesystem::path Recorder::GetRecordFilePath(CopyOperation op)
{
    auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
    auto const& folderOpt = s.GetLocalDataFolder();
    if (!folderOpt)
    {
        FC_LOG_ERROR(L"LocalDataFolder unavailable, skip recording.");
        return {};
    }

    auto const& folder = *folderOpt;
    return folder / std::format(L"{}{}.txt", toFlag(op), GetTimeString());
}

bool Recorder::HasRecord()
{
    try
    {
        auto& s = FastCopy::Settings::CommonSharedSettings::Instance();
        auto const& folderOpt = s.GetLocalDataFolder();
        if (!folderOpt) {
            FC_LOG_ERROR(L"LocalDataFolder unavailable, skip recording.");
            return false;
        }

        auto const& folder = *folderOpt;

        if (folder.empty())
        {
            FC_LOG_ERROR(L"LocalDataFolder empty, skip recording.");
            return false;
        }

        std::error_code ec;
        if (!std::filesystem::exists(folder, ec) || ec)
        {
            FC_LOG_WARN(L" folder '{}' not exists, ec={}", folder.wstring(), ec.value());
            return false;
        }

        for (auto it = std::filesystem::directory_iterator{ folder, ec };
            it != std::filesystem::directory_iterator{} && !ec;
            ++it)
        {
            auto name = it->path().filename().wstring();
            if (!name.empty() &&
                (name.starts_with(L'C') || name.starts_with(L"M2") || name.starts_with(L'P')))
            {
                FC_LOG_DEBUG(L" found '{}'", name);
                return true;
            }
        }

        if (ec)
        {
            FC_LOG_ERROR(L" iterate error, ec={}", ec.value());
        }

        return false;
    }
    catch (std::exception const& e)
    {
        FC_LOGA_ERROR("Recorder::HasRecord: exception: {}", e.what());
    }
    catch (...)
    {
        FC_LOG_ERROR(L"Recorder::HasRecord: unknown exception.");
    }

    return false;
}
