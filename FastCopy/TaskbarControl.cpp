#include "pch.h"
#include "TaskbarControl.h"
#if __has_include("TaskbarControl.g.cpp")
#include "TaskbarControl.g.cpp"
#endif
#include "Taskbar.h"
#include "WindowHelper.h"

namespace winrt::FastCopy::implementation
{
	double TaskbarControl::Value()
	{
		throw winrt::hresult_not_implemented{};
	}

	void TaskbarControl::Value(double value)
	{
		Taskbar::SetProgressValue(m_hwnd, value * 100.0);
	}

	winrt::Microsoft::UI::Xaml::Window TaskbarControl::Window()
	{
		throw winrt::hresult_not_implemented{};
	}

	void TaskbarControl::Window(winrt::Microsoft::UI::Xaml::Window const& value)
	{
		m_hwnd = GetHwnd(value);
	}
}
