#include "Config.h"

Config& Config::GetInstance()
{
    static Config s_instance;
    return s_instance;
}
