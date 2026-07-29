#include "pch.h"
#include "Notification.h"
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.Foundation.h>
#include <format>
#include <string>
#include <string_view>
#include "Settings.h"
#include "ResourceHelper.h"

//Minimal XML escaping for values interpolated into the toast payload.
static std::wstring xmlEscape(std::wstring_view text)
{
	std::wstring out;
	out.reserve(text.size());
	for (auto c : text)
	{
		switch (c)
		{
			case L'&':  out += L"&amp;";  break;
			case L'<':  out += L"&lt;";   break;
			case L'>':  out += L"&gt;";   break;
			case L'"':  out += L"&quot;"; break;
			case L'\'': out += L"&apos;"; break;
			default:    out += c;         break;
		}
	}
	return out;
}

//Builds the "fastcopy://open?<url-encoded path>" protocol URI carried by the toast's Open action.
//Clicking it makes Windows launch this packaged app through its registered "fastcopy" protocol,
//delivering the URI on the command line (handled by CommandLineHandler). This needs no COM
//activator and no WinAppSDK notification runtime, so it works on Windows 10 1809+ (build 17763).
static std::wstring makeOpenUri(winrt::hstring const& destination)
{
	auto const encoded = winrt::Windows::Foundation::Uri::EscapeComponent(destination);
	return std::format(L"fastcopy://open?{}", std::wstring_view{ encoded });
}

//Assembles a generic toast with the given body text, an "Open" action (protocol activation) and a
//system "Dismiss" action. The whole toast body also activates the Open URI when clicked. openButtonStyle
//is a hint-buttonStyle value ("Success"/"Critical") tinting the Open button; useButtonStyle on the
//toast is what enables those colors.
static std::wstring buildToastXml(winrt::hstring const& prompt, winrt::hstring const& destination, std::wstring_view openButtonStyle)
{
	auto const openUri = xmlEscape(makeOpenUri(destination));
	return std::format(
		LR"(<toast launch="{0}" activationType="protocol" useButtonStyle="true">)"
		LR"(<visual><binding template="ToastGeneric"><text>{1}</text></binding></visual>)"
		LR"(<actions>)"
		LR"(<action content="{2}" activationType="protocol" arguments="{0}" hint-buttonStyle="{4}"/>)"
		LR"(<action content="{3}" activationType="system" arguments="dismiss"/>)"
		LR"(</actions>)"
		LR"(</toast>)",
		openUri,
		xmlEscape(prompt),
		xmlEscape(GetStringResource(L"NotificationOpenButtonText")),
		xmlEscape(GetStringResource(L"NotificationDismissButtonText")),
		openButtonStyle
	);
}

static void showToast(std::wstring const& xml)
{
	winrt::Windows::Data::Xml::Dom::XmlDocument doc;
	doc.LoadXml(xml);
	winrt::Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier()
		.Show(winrt::Windows::UI::Notifications::ToastNotification{ doc });
}

namespace Notification
{
	/**

		Successfully <copied/moved/deleted> <num> items!
			---------  			---------
			| Open  |  			| Dismiss|
			---------  			---------
	*/
	void SendSuccess(winrt::hstring prompt, winrt::hstring destination)
	{
		if (!Settings{}.Get<bool>(Settings::Notify, true))
			return;

		showToast(buildToastXml(prompt, destination, L"Success"));
	}

	/**
		<copied/moved/deleted> <num> / <total>  items!
			---------   ---------
			| Open  |   | Dismiss |
			---------   ---------
	*/
	void SendFailed(winrt::hstring prompt, winrt::hstring destination)
	{
		showToast(buildToastXml(prompt, destination, L"Critical"));
	}
}
