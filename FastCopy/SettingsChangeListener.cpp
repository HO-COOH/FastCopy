#include "pch.h"
#include "SettingsChangeListener.h"
#include "Settings.h"

void SettingsChangeListener::OnThemeChange(std::function<void(ThemeChangeEventArg)> func)
{
	m_themeChangeHandler.push_back(std::move(func));
	createWindowIfNotCreated();
}

void SettingsChangeListener::BroadcastThemeChange()
{
	HWND findResult{};
	Settings const settings;
	ThemeChangeEventArg arg
	{
		.theme = settings.Get<int>(Settings::ThemeSelection, 0),
		.effect = settings.Get<int>(Settings::BackgroundSelection, 0)
	};
	//Post rather than send: the handlers set a backdrop, and TenMica calls out-of-process COM (IDesktopWallpaper),
	//which fails with RPC_E_CANTCALLOUT_ININPUTSYNCCALL while handling a message sent from another process
	do {
		 findResult = FindWindowEx(HWND_MESSAGE, findResult, MessageWindowClassName, L"");
		 if (findResult != NULL)
			 PostMessage(findResult, WM_SETTINGSCHANGED, static_cast<WPARAM>(arg.theme), static_cast<LPARAM>(arg.effect));
	} while (findResult != NULL);
}

SettingsChangeListener& SettingsChangeListener::GetInstance()
{
	static SettingsChangeListener s_instance;
	return s_instance;
}

void SettingsChangeListener::createWindowIfNotCreated()
{
	if (!m_hwnd)
		createWindow();
}

void SettingsChangeListener::createWindow()
{
	WNDCLASS ws{};
	ws.lpszClassName = MessageWindowClassName;
	ws.lpfnWndProc = +[](HWND hwnd, UINT message, WPARAM param, LPARAM lparam) -> LRESULT
	{
		if (message == WM_SETTINGSCHANGED)
		{
			ThemeChangeEventArg const themeChangeEventArg
			{
				.theme = static_cast<int>(param),
				.effect = static_cast<int>(lparam)
			};
			for (auto& handler : SettingsChangeListener::GetInstance().m_themeChangeHandler)
			{
				handler(themeChangeEventArg);
			}
		}
		return DefWindowProc(hwnd, message, param, lparam);
	};
	RegisterClass(&ws);
	m_hwnd = CreateWindow(MessageWindowClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);
	assert(m_hwnd);
}
