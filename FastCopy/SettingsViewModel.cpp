#include "pch.h"
#include "SettingsViewModel.h"
#if __has_include("SettingsViewModel.g.cpp")
#include "SettingsViewModel.g.cpp"
#endif



namespace winrt::FastCopy::implementation
{
	bool SettingsViewModel::Notify()
	{
		return false;
	}
	void SettingsViewModel::Notify(bool value)
	{
	}
	int SettingsViewModel::MultipleWindowBehavior()
	{
		return 0;
	}
	void SettingsViewModel::MultipleWindowBehavior(int value)
	{
	}
	int SettingsViewModel::ThemeSelection()
	{
		return 0;
	}
	void SettingsViewModel::ThemeSelection(int value)
	{
	}
	int SettingsViewModel::BackgroundSelection()
	{
		return 0;
	}
	void SettingsViewModel::BackgroundSelection(int value)
	{
	}
}
