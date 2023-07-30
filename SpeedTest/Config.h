#pragma once
#include <string>
#include "Env.h"
#include <chrono>
#include <atomic>

struct Config
{
public:
	static Config& GetInstance();

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
    std::string sourceFolder;
    std::string destinationFolder;

    void CreateSourceAndDestinationFolder() const;
private:
	Config() = default;
	Config(Config const&) = delete;
	Config(Config&&) = default;
	Config& operator=(Config const&) = delete;
	Config& operator==(Config&&) = delete;

    constexpr auto static SaveFilename = "Config.json";
public:
    //Use `cereal` to serialize & deserialize the config
    template<typename Archive>
    void serialize(Config& archive)
    {
        archive(
            copy_tab.bigFileSizeMB,
            move_tab.bigFileSizeMB,
            delete_tab.bigFileSizeMB,
            sourceFolder,
            destinationFolder
        );
    }

    static void LoadFromFile();
    static void SaveToFile();
};

std::wstring convert_to_wstring(const std::string& str);