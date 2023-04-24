#include "pch.h"
#include "DebugLogger.h"
#include <winrt/Windows.Storage.h>

Logger& Logger::GetInstance()
{
    static Logger s_instance;
    return s_instance;
}

static const std::string LogFilePath()
{
    auto const localFolderPath = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path() + LR"(\log.txt)";
    return winrt::to_string(localFolderPath);
}

Logger::Logger() : m_logger{spdlog::rotating_logger_st("testLogger", LogFilePath(), 102400, 1)}
{
    m_logger->set_pattern("[%H:%M:%S.%e] [%l] [t:%t] [%!:%#] %v");
    m_logger->flush_on(spdlog::level::info);
}
