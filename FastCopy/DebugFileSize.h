#pragma once
#include <string_view>
#include <filesystem>

void DebugFileSize(std::wstring_view path);
void DebugFileSize(std::filesystem::path const& path);