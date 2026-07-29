#pragma once

enum class AppLaunchMode
{
	//Launch by app icon, open settings window
	LaunchSettings,

	//Launch using protocol by shell extension, run copy
	LaunchCopy,

	//Launch by clicking a notification's "Open" action. Windows re-launches us through the
	//"fastcopy" protocol with "fastcopy://open?<url-encoded path>" on the command line
	OpenLocation
};
