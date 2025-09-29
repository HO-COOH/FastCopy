#include "Config.h"
#include <filesystem>
#include <fstream>
#include "ConfigArchive.h"

Config& Config::GetInstance()
{
    static Config s_instance;
    if (!s_instance.m_loaded)
    {
        LoadFromFile(s_instance);
        s_instance.m_loaded = true;
    }

    return s_instance;
}

void Config::LoadFromFile(Config& instance)
{
    if (!std::filesystem::exists(SaveFilename))
        return;

    ConfigArchive{ SaveFilename }.Load(instance);
}

void Config::SaveToFile(Config const& instance)
{
    ConfigArchive{ SaveFilename }.Save(instance);
}

bool Config::isSourcePathCorrect() const
{
    return false;
}

bool Config::isDestPathCorrect() const
{
    return false;
}
