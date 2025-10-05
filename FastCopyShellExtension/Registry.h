#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <Windows.h>
#include <wil/registry.h>

class Registry
{
	wil::unique_hkey m_key;
public:
	Registry(std::wstring_view subkey = L"fastcopy");

	//template<typename T>
	//void write(std::wstring_view name, T const& value)
	//{
	//	if constexpr (std::is_same_v<std::wstring, T> || std::is_same_v<std::wstring_view, T>)
	//	{
	//		wil::reg::set_value_string(m_key.get(), name.data(), value.data());
	//	}
	//	else
	//	{
	//		static_assert(false);
	//	}
	//}

	/*std::wstring read(std::wstring_view name)
	{
		DWORD bytes{};
		RegGetValueW(
			m_key.get(),
			nullptr,
			name.data(),
			RRF_RT_ANY,
			nullptr,
			nullptr,
			&bytes
		);
		std::wstring buffer((bytes / 2) - 1, {});
		RegGetValueW(
			m_key.get(),
			nullptr,
			name.data(),
			RRF_RT_ANY,
			nullptr,
			buffer.data(),
			&bytes
		);
		return buffer;
	}*/

	static std::wstring Record();
	static void Record(std::wstring_view fileName);
};

