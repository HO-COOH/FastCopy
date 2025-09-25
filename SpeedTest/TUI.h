#pragma once
#include <chrono>
#include <string>
#include "Config.h"
#include <ftxui/component/screen_interactive.hpp>

class TUI
{
    std::wstring m_tempSourceDir;
    std::wstring m_tempDestDir;
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
    void runCopyTab();

    void runImpl();
    void redraw();
public:
	static void Run();
};