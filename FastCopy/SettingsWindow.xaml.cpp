#include "pch.h"
#include "SettingsWindow.xaml.h"
#if __has_include("SettingsWindow.g.cpp")
#include "SettingsWindow.g.cpp"
#endif

#include "ViewModelLocator.h"
#include "UIDispatcher.h"


namespace winrt::FastCopy::implementation
{
    SettingsWindow::SettingsWindow()
    {
        UIDispatcher::g_dispatcher = DispatcherQueue();
        UIDispatcher::g_mainWindow = *this;
    }

    winrt::FastCopy::SettingsViewModel SettingsWindow::ViewModel()
    {
        return ViewModelLocator::GetInstance().SettingsViewModel();
    }
}
