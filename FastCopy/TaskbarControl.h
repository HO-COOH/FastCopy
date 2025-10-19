#pragma once

#include "TaskbarControl.g.h"

namespace winrt::FastCopy::implementation
{
    struct TaskbarControl : TaskbarControlT<TaskbarControl>
    {
        TaskbarControl() = default;

        double Value();
        void Value(double value);

        winrt::Microsoft::UI::Xaml::Window Window();
        void Window(winrt::Microsoft::UI::Xaml::Window const& value);

        FastCopy::TaskbarState State();
		void State(FastCopy::TaskbarState value);
    private:
        HWND m_hwnd;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct TaskbarControl : TaskbarControlT<TaskbarControl, implementation::TaskbarControl>
    {
    };
}
