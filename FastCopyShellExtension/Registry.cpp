#include "Registry.h"
#include <format>
#include <wil/registry.h>

Registry::Registry(std::wstring_view subkey) : 
	m_key{wil::reg::create_unique_key(HKEY_CURRENT_USER, std::format(LR"(Software\{})", subkey).data(), wil::reg::key_access::readwrite)}
{
}

std::wstring Registry::Record()
{
	Registry reg;
	return wil::reg::get_value_string<std::wstring>(reg.m_key.get(), L"record");
}

void Registry::Record(std::wstring_view fileName)
{
	Registry reg;
	wil::reg::set_value_string(reg.m_key.get(), L"record", fileName.data());
}
