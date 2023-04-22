#pragma once
#include <string_view>
struct DebugLogger
{
	void Log(std::wstring_view arg);

	static DebugLogger& GetInstance();
};

#define LOGI(...) DebugLogger::GetInstance().Log(__VA_ARGS__)