#pragma once

#include "WindowEx.g.h"
#include "WindowHelper.h"
#include <winrt/Microsoft.UI.Windowing.h>

namespace winrt::FastCopy::implementation
{
    struct WindowEx : WindowExT<WindowEx>
    {
        WindowEx() = default;

        bool IsMaximizable();
        void IsMaximizable(bool value);

        bool IsMinimizable();
        void IsMinimizable(bool value);

        bool IsModal();
        void IsModal(bool value);

        bool IsResizable();
        void IsResizable(bool value);

        winrt::Microsoft::UI::Windowing::AppWindow AppWindow();
    private:
        winrt::Microsoft::UI::Windowing::AppWindow appWindow = GetAppWindow(*this);
        winrt::Microsoft::UI::Windowing::OverlappedPresenter presenter = appWindow.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct WindowEx : WindowExT<WindowEx, implementation::WindowEx>
    {
    };
}
