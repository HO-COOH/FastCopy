#pragma once
#include <string>
#include "resource.h"
#include "CopyOperation.h"

class IconProvider
{
public:
	static IconProvider const& GetForCurrentTheme();
	
	int GetForOperation(CopyOperation op) const;
private:
	enum class ThemeType
	{
		Light,
		Dark
	} m_themeType;
	IconProvider(ThemeType type) : m_themeType{ type } {}
};