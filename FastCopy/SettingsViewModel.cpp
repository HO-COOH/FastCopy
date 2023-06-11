#include "pch.h"
#include "SettingsViewModel.h"
#if __has_include("SettingsViewModel.g.cpp")
#include "SettingsViewModel.g.cpp"
#endif
#include "Global.h"
#include "SettingsChangeListener.h"

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
		Global::windowEffectHelper.SetTheme(static_cast<winrt::Microsoft::UI::Xaml::ElementTheme>(value));
		SettingsChangeListener::GetInstance().BroadcastThemeChange();
	}
	int SettingsViewModel::BackgroundSelection()
	{
		return m_model.Get<int>(Settings::BackgroundSelection, 0);
	}
	void SettingsViewModel::BackgroundSelection(int value)
	{
		m_model.Set(Settings::BackgroundSelection, value);
		switch (value)
		{
			case 0:
				Global::windowEffectHelper.TrySetMica();
				break;
			case 1:
				Global::windowEffectHelper.TrySetAcrylic();
				break;
			case 2:
				Global::windowEffectHelper.Reset();
				break;
			default:
				break;
		}
		SettingsChangeListener::GetInstance().BroadcastThemeChange();
	}
}
