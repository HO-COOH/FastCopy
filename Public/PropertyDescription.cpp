#include "pch.h"
#include "PropertyDescription.h"
#include <propsys.h>
#pragma comment(lib, "propsys.lib")

PropertyDescription::PropertyDescription(PROPERTYKEY key)
{
	winrt::check_hresult(PSGetPropertyDescription(key, IID_PPV_ARGS(m_ptr.put())));
}

wil::unique_cotaskmem_string PropertyDescription::GetDisplayName()
{
	wil::unique_cotaskmem_string result;
	winrt::check_hresult(m_ptr->GetDisplayName(&result));
	return result;
}
