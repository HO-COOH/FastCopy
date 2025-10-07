#include "pch.h"
#include "App.xaml.h"
#include "Console.h"

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Console::Create();
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    ::winrt::Microsoft::UI::Xaml::Application::Start(
        [](auto&&)
        {
            ::winrt::make<::winrt::FastCopy::implementation::App>();
        });

    return 0;
}