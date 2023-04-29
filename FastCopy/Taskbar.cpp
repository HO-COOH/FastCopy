#include "pch.h"
#include "Taskbar.h"

#if __has_include("winrt/base.h")
#include <winrt/base.h>
#define FastCopy_UseWinRT
#endif

ITaskbarList3* Taskbar::getPtr()
{
#ifdef FastCopy_UseWinRT
    static winrt::com_ptr<ITaskbarList3> instance = []
    {
        winrt::com_ptr<ITaskbarList3> ptr;
        CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, __uuidof(ITaskbarList3), ptr.put_void());
        return ptr;
    }();
    return instance.get();
#endif
}

void Taskbar::SetProgressState(HWND hwnd, ProgressState state)
{
    getPtr()->SetProgressState(hwnd, static_cast<TBPFLAG>(state));
}

void Taskbar::SetProgressValue(HWND hwnd, ULONGLONG current, ULONGLONG maximum)
{
    getPtr()->SetProgressValue(hwnd, current, maximum);
}
