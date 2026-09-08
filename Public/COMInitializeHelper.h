#pragma once
#include <combaseapi.h>
#include <wil/resource.h>

class COMInitializeHelper
{
	wil::unique_couninitialize_call m_uninitialize{ wil::CoInitializeEx(COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE) };
public:
	COMInitializeHelper() = default;

	COMInitializeHelper(COMInitializeHelper const&) = delete;
	COMInitializeHelper(COMInitializeHelper&&) = delete;
	COMInitializeHelper& operator=(COMInitializeHelper const&) = delete;
	COMInitializeHelper& operator=(COMInitializeHelper&&) = delete;
};
