#pragma once
#include <string>
#include <string_view>
#include <type_traits>
#include <Windows.h>
#include <wil/registry.h>

class Registry
{
	wil::unique_hkey m_key;
	Registry(std::wstring_view subkey = L"fastcopy");
public:
	static std::wstring Record();
	static void Record(std::wstring_view fileName);
};

