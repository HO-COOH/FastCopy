#pragma once
#include <Windows.h>
#include <string_view>
#include "AppLaunchMode.h"

//We are extracting some App.OnLaunched() logic to this class
//Benefits include optimizing for operations that does not need to constructing the App instance
//Like opening the destination folder, and for dismissing the toast notification
class CommandLineHandler
{
	PWSTR m_cmd;
public:
	static inline AppLaunchMode AppLaunchMode{ AppLaunchMode::LaunchSettings };
	
	CommandLineHandler(PWSTR cmd);
	void Run();

	static void handleToastActivation(std::wstring_view arguments);
};
