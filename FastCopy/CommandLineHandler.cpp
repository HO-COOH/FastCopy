#include "pch.h"
#include "CommandLineHandler.h"
#include <string_view>
#include <shellapi.h>
#include "App.xaml.h"
#include "Console.h"
#include "COMInitializeHelper.h"
#include "CommandLine.h"
#include "CommonSharedSettings.h"

static std::pair<std::wstring_view, std::wstring_view> parseToastArgument(std::wstring_view argument)
{
	auto const index = argument.find(L'=');
	return { argument.substr(0, index), argument.substr(index + 1) };
}

CommandLineHandler::CommandLineHandler(PWSTR cmd) : m_cmd{ cmd }
{
	std::wstring_view view{ m_cmd };
	if (view.starts_with(L"fastcopy://"))
		AppLaunchMode = AppLaunchMode::LaunchCopy;
	else if (view.starts_with(L"open="))
		AppLaunchMode = AppLaunchMode::OpenLocation;
	else if (view.starts_with(L"dismiss"))
		AppLaunchMode = AppLaunchMode::Dismiss;
}

void CommandLineHandler::Run()
{
	switch (AppLaunchMode)
	{
		case AppLaunchMode::LaunchSettings: 
			if (winrt::FastCopy::implementation::App::HasAnotherInstance())
				return;

			[[fallthrough]];
		case AppLaunchMode::LaunchCopy:
		{
			Console::Create();
			Command::Get().Set(m_cmd);
			winrt::init_apartment(winrt::apartment_type::single_threaded);
			winrt::Microsoft::UI::Xaml::Application::Start(
				[](auto&&)
				{
					winrt::make<::winrt::FastCopy::implementation::App>();
				});
			return;
		}
		case AppLaunchMode::OpenLocation:
		{
			/*
				eg:
					open=F:/testDestination
					dismiss=
			*/
			COMInitializeHelper COMInit;
			auto const [action, arg] = parseToastArgument(m_cmd);
			if (action == L"open")
			{
				ShellExecuteW(
					nullptr,
					L"open",
					arg.data(),
					nullptr,
					nullptr,
					SW_SHOWNORMAL
				);
			}
			return;
		}
		case AppLaunchMode::Dismiss:
			return;
	}
}
