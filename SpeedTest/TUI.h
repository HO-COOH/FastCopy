#pragma once
#include <chrono>
#include <string>
#include "Config.h"
#include <ftxui/component/screen_interactive.hpp>

class TUI
{
    std::string m_tempSourceDir = Config::GetInstance().sourceFolder;
    std::string m_tempDestDir = Config::GetInstance().destinationFolder;
    ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
    void runCopyTab();

    void runImpl();
    void redraw();
public:
	static void Run();
};