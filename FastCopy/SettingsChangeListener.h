#pragma once
#include <functional>
#include <vector>
/**
 * @brief Handling sending & receiving settings changed message to all created process
 */
class SettingsChangeListener
{
public:
	/**
	 * @brief Settings changed data
	 */
	struct ThemeChangeEventArg
	{
		int theme = -1;
		int effect = -1;
	};

	/**
	 * @brief Add handler for handling settings change message
	 * 
	 * @param func The handler
	 */
	void OnThemeChange(std::function<void(ThemeChangeEventArg)> func);

	/**
	 * @brief Send settings change message to all FastCopy window
	 * 
	 */
	void BroadcastThemeChange();

	static SettingsChangeListener& GetInstance();

private:
	constexpr static auto MessageWindowClassName = L"FastcopyMessageWindowClass";
	constexpr static auto WM_SETTINGSCHANGED = WM_USER + 10;
	std::vector<std::function<void(ThemeChangeEventArg)>> m_themeChangeHandler;
	HWND m_hwnd{};
	void createWindowIfNotCreated();
	void createWindow();
};

