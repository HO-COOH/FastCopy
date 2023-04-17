#pragma once
#include <winrt/Microsoft.UI.Dispatching.h>

struct Global
{
	static inline winrt::Microsoft::UI::Dispatching::DispatcherQueue UIThread{ nullptr };
};

