#include "pch.h"
#include "DebugLogger.h"
#include <winrt/Windows.Storage.h>
#include <chrono>
#include <format>

Logger& Logger::GetInstance()
{
    static Logger s_instance;
    return s_instance;
}

static const std::string LogFilePath()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    // Format timestamp as YYYY-MM-DD-HH-mm-ss
    auto timestamp = std::format(L"\\{:04d}-{:02d}-{:02d}-{:02d}-{:02d}-{:02d}.txt",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);
    
    auto const localFolderPath = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path() + timestamp;
    return winrt::to_string(localFolderPath);
}

Logger::Logger() : m_logger{spdlog::rotating_logger_st("testLogger", LogFilePath(), 102400, 1)}
{
    m_logger->set_pattern("[%H:%M:%S.%e] [%l] [t:%t] [%!:%#] %v");
    m_logger->flush_on(spdlog::level::info);
}
