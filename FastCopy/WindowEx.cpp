#include "pch.h"
#include "WindowEx.h"
#if __has_include("WindowEx.g.cpp")
#include "WindowEx.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
    bool WindowEx::IsMaximizable()
    {
        return presenter.IsMaximizable();
    }
    void WindowEx::IsMaximizable(bool value)
    {
        presenter.IsMaximizable(value);
    }
    bool WindowEx::IsMinimizable()
    {
        return presenter.IsMinimizable();
    }
    void WindowEx::IsMinimizable(bool value)
    {
        presenter.IsMinimizable(value);
    }
    bool WindowEx::IsModal()
    {
        return presenter.IsModal();
    }
    void WindowEx::IsModal(bool value)
    {
        presenter.IsModal(value);
    }
    bool WindowEx::IsResizable()
    {
        return presenter.IsResizable();
    }
    void WindowEx::IsResizable(bool value)
    {
        presenter.IsResizable(value);
    }
    winrt::Microsoft::UI::Windowing::AppWindow WindowEx::AppWindow()
    {
        return appWindow;
    }
}
