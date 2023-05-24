#include "pch.h"
#include "SettingsViewModel.h"
#if __has_include("SettingsViewModel.g.cpp")
#include "SettingsViewModel.g.cpp"
#endif


namespace winrt::FastCopy::implementation
{
	bool SettingsViewModel::Notify()
	{
		return m_model.Get<bool>(Settings::Notify, true);
	}
	void SettingsViewModel::Notify(bool value)
	{
		m_model.Set(Settings::Notify, value);
	}
	int SettingsViewModel::MultipleWindowBehavior()
	{
		return m_model.Get<int>(Settings::MultipleWindowBehavior, 0);
	}
	void SettingsViewModel::MultipleWindowBehavior(int value)
	{
		m_model.Set(Settings::MultipleWindowBehavior, value);
	}
	int SettingsViewModel::ThemeSelection()
	{
		return m_model.Get<int>(Settings::ThemeSelection, 0);
	}
	void SettingsViewModel::ThemeSelection(int value)
	{
		m_model.Set(Settings::ThemeSelection, value);
	}
	int SettingsViewModel::BackgroundSelection()
	{
		return m_model.Get<int>(Settings::BackgroundSelection, 0);
	}
	void SettingsViewModel::BackgroundSelection(int value)
	{
		m_model.Set(Settings::BackgroundSelection, value);
	}
}
