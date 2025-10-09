#pragma once

enum class AppLaunchMode
{
	//Launch by app icon, open settings window
	LaunchSettings,

	//Launch using protocol by shell extension, run copy
	LaunchCopy,

	//Launch by notification popup, open a folder
	OpenLocation,

	//Launch by notification popup, do nothing and exit
	Dismiss
};
