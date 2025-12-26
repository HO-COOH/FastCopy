#pragma once
#include <winrt/base.h>
#include <wtypes.h>
#include <wil/com.h>

struct IPropertyDescription;

class PropertyDescription
{
	winrt::com_ptr<IPropertyDescription> m_ptr;
public:
	PropertyDescription(PROPERTYKEY key);

	wil::unique_cotaskmem_string GetDisplayName();
};