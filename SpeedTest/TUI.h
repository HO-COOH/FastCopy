#pragma once
#include <chrono>
#include <string>
#include <atomic>

struct TestData
{
    struct TabData
    {
        struct Implementations
        {
            bool generate{};

            struct ImplementationResult
            {
                bool selected{};
                //std::chrono::milliseconds duration{};
                std::atomic_bool started{};
                std::chrono::steady_clock::time_point startTime{};
                std::chrono::steady_clock::time_point endTime{};
            }
            COM_selected,
            win32_selected,
            std_selected,
            xcopy_selected,
            robocopy_selected;
        } fourK, bigFile;


        int bigFileSizeMB = 2048;
    } copy_tab, move_tab, delete_tab;
    std::string sourceDir;
    std::string destDir;
};

class TUI
{
    TestData data;

    void runCopyTab();

    void runImpl();
public:
	static void Run();
};
