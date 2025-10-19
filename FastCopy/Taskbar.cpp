#include "pch.h"
#include "Taskbar.h"

ITaskbarList3* Taskbar::getPtr()
{
    static winrt::com_ptr<ITaskbarList3> instance = []
    {
        winrt::com_ptr<ITaskbarList3> ptr;
        CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL, __uuidof(ITaskbarList3), ptr.put_void());
        return ptr;
    }();
    return instance.get();
}

void Taskbar::SetProgressState(HWND hwnd, TBPFLAG state)
{
    getPtr()->SetProgressState(hwnd, state);
}

void Taskbar::SetProgressValue(HWND hwnd, ULONGLONG current, ULONGLONG maximum)
{
    getPtr()->SetProgressValue(hwnd, current, maximum);
}
