#pragma once
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.h>
#include <type_traits>

struct Settings
{
	/*Keys for settings*/
	constexpr static auto Notify = L"Notify";
	constexpr static auto MultipleWindowBehavior = L"MultipleWindowBehavior";
	constexpr static auto ThemeSelection = L"ThemeSelection";
	constexpr static auto BackgroundSelection = L"BackSelection";

	/**
	 * @brief Get settings value of a specific type, will throw if the key is not found
	 * @tparam T value type of the setting
	 * @param key name of the setting
	 */
	template<typename T>
	auto Get(wchar_t const* key) const
	{
		if constexpr (std::is_base_of_v<winrt::Windows::Foundation::IInspectable, std::remove_cvref_t<T>>)
			return m_map.Lookup(key);
		else
			return winrt::unbox_value<T>(m_map.Lookup(key));
	}

	/**
	 * @brief Get settings value of a specific type, return a default value if not found
	 * @tparam T value type of the setting
	 * @param key name of the setting
	 * @param defaultValue that will be returned if the setting is not set
	 */
	template<typename T>
	auto Get(wchar_t const* key, T&& defaultValue) const
	{
		if constexpr (std::is_base_of_v<winrt::Windows::Foundation::IInspectable, std::remove_cvref_t<T>>)
			return m_map.TryLookup(key);
		else
			return winrt::unbox_value_or<T>(m_map.TryLookup(key), defaultValue);
	}

	/**
	 * @brief 
	 * 
	 * @param key
	 * @param value
	 */
	template<typename T>
	void Set(wchar_t const* key, T&& value)
	{
		if constexpr (std::is_base_of_v<winrt::Windows::Foundation::IInspectable, std::remove_cvref_t<T>>)
			m_map.Insert(key, value);
		else
			m_map.Insert(key, winrt::box_value(value));
	}
private:
	winrt::Windows::Foundation::Collections::IPropertySet m_map = winrt::Windows::Storage::ApplicationData::Current().LocalSettings().Values();
};

