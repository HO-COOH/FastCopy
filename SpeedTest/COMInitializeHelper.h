#pragma once
#include <combaseapi.h>
#include <stdexcept>
struct COMInitializeHelper
{
	COMInitializeHelper()
	{
		if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
			throw std::exception{ "COM initialize failed" };
	}

	~COMInitializeHelper()
	{
		CoUninitialize();
	}

	COMInitializeHelper(COMInitializeHelper const&) = delete;
	COMInitializeHelper(COMInitializeHelper&&) = delete;
	COMInitializeHelper& operator=(COMInitializeHelper const&) = delete;
	COMInitializeHelper& operator=(COMInitializeHelper&&) = delete;
};
