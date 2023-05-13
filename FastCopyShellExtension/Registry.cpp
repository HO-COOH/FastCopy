#include "Registry.h"
#include <format>

Registry::Registry(std::wstring const& subkey)
{
	RegCreateKeyW(
		HKEY_CURRENT_USER,
		std::format(LR"(Software\{})", subkey).data(),
		&m_key
	);
}
