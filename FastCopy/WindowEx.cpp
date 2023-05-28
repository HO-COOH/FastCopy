#include "pch.h"
#include "WindowEx.h"
#if __has_include("WindowEx.g.cpp")
#include "WindowEx.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
    bool WindowEx::IsMaximizable()
    {
        return true;
    }
    void WindowEx::IsMaximizable(bool value)
    {
        OutputDebugString(std::to_wstring(value).data());
    }
    bool WindowEx::IsMinimizable()
    {
        return false;
    }
    void WindowEx::IsMinimizable(bool value)
    {
    }
    bool WindowEx::IsModal()
    {
        return false;
    }
    void WindowEx::IsModal(bool value)
    {
    }
    bool WindowEx::IsResizable()
    {
        return false;
    }
    void WindowEx::IsResizable(bool value)
    {
    }
}
