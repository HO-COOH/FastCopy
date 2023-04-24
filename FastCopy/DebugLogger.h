#pragma once
#include <string_view>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
struct Logger
{
public:
	decltype(spdlog::rotating_logger_st("", "", 1, 1)) m_logger;

	static Logger& GetInstance();
private:
	Logger();
};

#define LOGI(...) Logger::GetInstance().m_logger->log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__}, spdlog::level::info, __VA_ARGS__)