#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <Windows.h>

class Registry
{
	HKEY m_key{};
public:
	Registry(std::wstring const& subkey = L"fastcopy");

	template<typename T>
	void write(std::wstring_view name, T const& value)
	{
		if constexpr (std::is_same_v<std::wstring, T> || std::is_same_v<std::wstring_view, T>)
		{
			RegSetValueExW(
				m_key,
				name.data(),
				0,
				REG_SZ,
				reinterpret_cast<const BYTE*>(value.data()),
				(value.size() + 1) * 2
			);
		}
	}

	std::wstring read(std::wstring_view name)
	{
		DWORD bytes{};
		RegGetValueW(
			m_key,
			nullptr,
			name.data(),
			RRF_RT_ANY,
			nullptr,
			nullptr,
			&bytes
		);
		std::wstring buffer((bytes / 2) - 1, {});
		RegGetValueW(
			m_key,
			nullptr,
			name.data(),
			RRF_RT_ANY,
			nullptr,
			buffer.data(),
			&bytes
		);
		return buffer;
	}
};

