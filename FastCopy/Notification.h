#pragma once
#include <winrt/base.h>
namespace Notification
{
	/**
		
		Successfully <copied/moved/deleted> <num> items!
			---------  			---------
			| Open  |  			| Dismiss|    
			---------  			---------
	*/
	void SendSuccess(winrt::hstring prompt, winrt::hstring destination);

	/**
		<copied/moved/deleted> <num> / <total>  items!
			---------   ---------  ---------
			| Open  |   | Retry |  | Dismiss |
			---------   ---------  ---------
	*/
	void SendFailed(winrt::hstring prompt, winrt::hstring destination);
};



