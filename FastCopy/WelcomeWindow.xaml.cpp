#include "pch.h"
#include "WelcomeWindow.xaml.h"
#if __has_include("WelcomeWindow.g.cpp")
#include "WelcomeWindow.g.cpp"
#endif
#include <SettingsWindow.xaml.h>
#include "Global.h"
#include "WindowHelper.h"
#include <PackageConfig.h>
#include <wil/win32_helpers.h>
#include <filesystem>
#include "TextBlockClipAnimation.h"
#include "FastcopySettingsSingleInstanceLock.h"
#include "WindowEffectHelper.h"

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
	WelcomeWindow::WelcomeWindow(WindowEffectHelper& windowEffectHelper) : m_windowEffectHelper{ windowEffectHelper }
	{
		CenterWindow(Window(), winrt::Windows::Graphics::SizeInt32{
			.Width = PackageConfig::GetDefaultLanguage() == L"zh-cn" ? 500 : 600,
			.Height = 280
		});
		checkRobocopy();
		FastcopySettingsSingleInstanceLock::SetWindow(*this);
	}

	void WelcomeWindow::InitializeComponent()
	{
		WelcomeWindowT::InitializeComponent();
		TextBlockClipAnimation{}.SetImplicitAnimations(SubmitIssuesContent());
	}

	void WelcomeWindow::Button_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		winrt::make_self<SettingsWindow>(m_windowEffectHelper)->Activate();
		Close();
	}

	void winrt::FastCopy::implementation::WelcomeWindow::KillExplorerButton_Click(
		winrt::Windows::Foundation::IInspectable const&, 
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		system("taskkill /F /IM explorer.exe & start explorer");
		revealSubmitIssues();

	}
	winrt::FastCopy::RobocopyCheckResult WelcomeWindow::CheckResult()
	{
		return m_checkResult;
	}

	void WelcomeWindow::revealSubmitIssues()
	{
		SubmitIssuesContent().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
	}

	void WelcomeWindow::checkResult(winrt::FastCopy::RobocopyCheckResult value)
	{
		m_checkResult = value;
	}

	void WelcomeWindow::checkRobocopy()
	{
		//check if robocopy.exe exist in %systemroot%\System32
		if (!std::filesystem::exists(wil::ExpandEnvironmentStringsW(LR"(%systemroot%\System32\robocopy.exe)").get()))
		{
			checkResult(winrt::FastCopy::RobocopyCheckResult::NotFound);
			return;
		}

		//check if english resource exist in %systemroot%\System32\en-US\robocopy.exe.mui
		if (!std::filesystem::exists(wil::ExpandEnvironmentStringsW(LR"(%systemroot%\System32\en-US\robocopy.exe.mui)").get()))
		{
			checkResult(winrt::FastCopy::RobocopyCheckResult::NoEnglishResource);
			return;
		}

		checkResult(winrt::FastCopy::RobocopyCheckResult::Normal);
	}
}
