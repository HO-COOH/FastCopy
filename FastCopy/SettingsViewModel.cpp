#include "pch.h"
#include "SettingsViewModel.h"
#if __has_include("SettingsViewModel.g.cpp")
#include "SettingsViewModel.g.cpp"
#endif
#include "Global.h"
#include "SettingsChangeListener.h"
#include "RenameUtils.h"

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
	int SettingsViewModel::RenameBehavior()
	{
		return m_model.Get<int>(Settings::RenameBehavior, 1);
	}
	void SettingsViewModel::RenameBehavior(int value)
	{
		m_model.Set(Settings::RenameBehavior, value);
	}
	winrt::hstring SettingsViewModel::RenameSuffix()
	{
		auto value = m_model.Get<winrt::hstring>(Settings::RenameSuffix, L"");
		if(value != L"")
			return value;

		auto defaultValue = Utils::GetDefaultRenameSuffix();
		m_model.Set(Settings::RenameSuffix, defaultValue.data());
		return winrt::hstring{ std::move(defaultValue) };
	}
	void SettingsViewModel::RenameSuffix(winrt::hstring const& value)
	{
		Utils::IsRenameSuffixValid(value) ?
			m_model.Set(Settings::RenameSuffix, value) :
			raisePropertyChange(L"RenameSuffix");
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
		Global::windowEffectHelper.SetEffect(value);
		SettingsChangeListener::GetInstance().BroadcastThemeChange();
	}
	bool SettingsViewModel::DevMode()
	{
		return m_model.Get(Settings::DevMode, false);
	}
	void SettingsViewModel::DevMode(bool value)
	{
		m_model.Set(Settings::DevMode, value);
	}
	winrt::Microsoft::UI::Xaml::Visibility SettingsViewModel::IsRenameTextBoxVisible(int renameBehavior)
	{
		return renameBehavior == 1 ? winrt::Microsoft::UI::Xaml::Visibility::Visible : winrt::Microsoft::UI::Xaml::Visibility::Collapsed;
	}
}
