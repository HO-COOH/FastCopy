#include "pch.h"
#include "Notification.h"
#include <winrt/Microsoft.Windows.AppNotifications.Builder.h>
#include <winrt/Microsoft.Windows.AppNotifications.h>
#include <format>
#include "Settings.h"

namespace Notification
{
	static void AddButtonTo(
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationBuilder& builder,
		winrt::hstring const& text,
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButtonStyle style = winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButtonStyle::Default)
	{
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButton button{ text };
		button.SetButtonStyle(style);
		builder.AddButton(button);
	}

	static void AddButtonTo(
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationBuilder& builder,
		winrt::hstring const& text,
		winrt::hstring argumentName,
		winrt::hstring argumentValue,
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButtonStyle style = winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButtonStyle::Default)
	{
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButton button{ text };
		button.AddArgument(argumentName, argumentValue);
		button.SetButtonStyle(style);
		builder.AddButton(button);
	}

	/**

		Successfully <copied/moved/deleted> <num> items!
			---------  			---------
			| Open  |  			| Dismiss|
			---------  			---------
	*/
	void Notification::SendSuccess(winrt::hstring prompt, winrt::hstring destination)
	{
		if (!Settings{}.Get<bool>(Settings::Notify, true))
			return;

		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationBuilder builder;
		builder.AddText(prompt);
		
		AddButtonTo(builder, L"Open", L"open", destination, winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButtonStyle::Success);
		AddButtonTo(builder, L"Dismiss");
		winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().Show(builder.BuildNotification());
	}

	/**
		<copied/moved/deleted> <num> / <total>  items!
			---------   ---------  ---------
			| Open  |   | Retry |  | Dismiss |
			---------   ---------  ---------
	*/
	void SendFailed(winrt::hstring prompt, winrt::hstring destination)
	{
		winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationBuilder builder;
		builder.AddText(prompt);

		AddButtonTo(builder, L"Open", L"open", destination, winrt::Microsoft::Windows::AppNotifications::Builder::AppNotificationButtonStyle::Critical);
		AddButtonTo(builder, L"Retry", L"retry", destination);
		AddButtonTo(builder, L"Dismiss");
		winrt::Microsoft::Windows::AppNotifications::AppNotificationManager::Default().Show(builder.BuildNotification());
	}
}