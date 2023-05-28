#include "Config.h"
#include <filesystem>

Config& Config::GetInstance()
{
    static Config s_instance;
    return s_instance;
}

void Config::CreateSourceAndDestinationFolder() const
{
    std::filesystem::create_directory(sourceFolder);
    std::filesystem::create_directory(destinationFolder);
    if (!std::filesystem::exists(sourceFolder) || !std::filesystem::exists(destinationFolder))
        throw std::runtime_error{ "Error in creating folders" };
}
