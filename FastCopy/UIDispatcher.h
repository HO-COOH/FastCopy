#pragma once
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.h>
struct UIDispatcher
{
	static inline winrt::Microsoft::UI::Dispatching::DispatcherQueue g_dispatcher{ nullptr };
	static inline winrt::Microsoft::UI::Xaml::Window g_mainWindow{ nullptr };
};

