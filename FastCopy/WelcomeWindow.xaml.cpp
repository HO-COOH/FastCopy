#include "pch.h"
#include "WelcomeWindow.xaml.h"
#if __has_include("WelcomeWindow.g.cpp")
#include "WelcomeWindow.g.cpp"
#endif
#include <SettingsWindow.xaml.h>
#include "Global.h"


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
	WelcomeWindow::WelcomeWindow()
	{
		ExtendsContentIntoTitleBar(true);
	}


	void WelcomeWindow::Button_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		Global::settingsWindow = make<SettingsWindow>();
		Global::settingsWindow.Activate();
		Global::windowEffectHelper.SetTarget(Global::settingsWindow);
		Close();
	}

}
