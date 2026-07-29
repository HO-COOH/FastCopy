#include "pch.h"
#include "CommandLineHandler.h"
#include <string_view>
#include <shellapi.h>
#include <winrt/Windows.Foundation.h>
#include "App.xaml.h"
#include "Console.h"
#include "COMInitializeHelper.h"
#include "CommandLine.h"

CommandLineHandler::CommandLineHandler(PWSTR cmd) : m_cmd{ cmd }
{
	std::wstring_view view{ m_cmd };
	//A toast "Open" action re-launches us through the "fastcopy" protocol with an "open" authority.
	//Windows normalizes the URI (eg "fastcopy://open?x" is delivered as "fastcopy://open/?x"), so
	//match the "open" authority rather than an exact prefix, and check it before the generic copy
	//prefix (copy URIs look like fastcopy://"<dest>"|...) so we do not treat it as a copy job.
	if (view.starts_with(L"fastcopy://open"))
		AppLaunchMode = AppLaunchMode::OpenLocation;
	else if (view.starts_with(L"fastcopy://"))
		AppLaunchMode = AppLaunchMode::LaunchCopy;
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
			COMInitializeHelper COMInit;
			handleToastActivation(m_cmd);
			return;
		}
	}
}

void CommandLineHandler::HandleToastActivation(std::wstring_view arguments)
{
	//arguments is the full protocol command line, eg "fastcopy://open/?E%3A%2Fdest"
	auto const queryStart = arguments.find(L'?');
	if (queryStart == std::wstring_view::npos)
		return;

	auto const path = winrt::Windows::Foundation::Uri::UnescapeComponent(
		winrt::hstring{ arguments.substr(queryStart + 1) }
	);
	ShellExecuteW(
		nullptr,
		L"open",
		path.data(),
		nullptr,
		nullptr,
		SW_SHOWNORMAL
	);
}
