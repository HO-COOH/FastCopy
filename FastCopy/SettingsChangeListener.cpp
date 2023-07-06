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
	do {
		 findResult = FindWindowEx(HWND_MESSAGE, findResult, MessageWindowClassName, L"");
		 if (findResult != NULL)
		 {
			 COPYDATASTRUCT data{};
			 data.cbData = sizeof(data);
			 data.lpData = reinterpret_cast<PVOID>(&arg);
			 SendMessage(findResult, WM_COPYDATA, NULL, reinterpret_cast<LPARAM>(&data));
		 }
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
		if (message == WM_COPYDATA)
		{
			auto data = reinterpret_cast<COPYDATASTRUCT*>(lparam);
			auto themeChangeEventArg = reinterpret_cast<ThemeChangeEventArg*>(data->lpData);
			for (auto& handler : SettingsChangeListener::GetInstance().m_themeChangeHandler)
			{
				handler(*themeChangeEventArg);
			}
		}
		return DefWindowProc(hwnd, message, param, lparam);
	};
	RegisterClass(&ws);
	m_hwnd = CreateWindow(MessageWindowClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, nullptr, nullptr);
	assert(m_hwnd);
}
