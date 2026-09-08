#pragma once

#include <Windows.h>

#include <filesystem>
#include <optional>

[[nodiscard]] bool IsExplorerWindow(HWND window);
[[nodiscard]] std::optional<std::filesystem::path> GetExplorerFolder(HWND expectedForegroundWindow);
