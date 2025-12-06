#pragma once
#include <Windows.h>

class HostProcessHook
{
public:
    static bool Install();
    static void Uninstall();
};
