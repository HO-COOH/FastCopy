#pragma once
#include "WebBrowser2.h"
#include <ShlObj.h>

WebBrowser2::WebBrowser2(wil::com_ptr<IWebBrowser2>&& ptr) : m_ptr{ std::move(ptr) }
{
}

HWND WebBrowser2::HWND()
{
	::HWND value{};
	THROW_IF_FAILED(m_ptr->get_HWND(reinterpret_cast<SHANDLE_PTR*>(&value)));
	return value;
}

wil::unique_bstr WebBrowser2::LocationName()
{
	wil::unique_bstr value;
	THROW_IF_FAILED(m_ptr->get_LocationName(value.put()));
	return value;
}

wil::unique_bstr WebBrowser2::LocationURL()
{
	wil::unique_bstr value;
	THROW_IF_FAILED(m_ptr->get_LocationURL(value.put()));
	return value;
}

bool WebBrowser2::Visible()
{
	VARIANT_BOOL visible{};
	THROW_IF_FAILED(m_ptr->get_Visible(&visible));
	return static_cast<bool>(visible);
}

bool WebBrowser2::IsParentOrSelf(::HWND hwnd)
{
	if (!hwnd) return false;

	wil::com_ptr<IServiceProvider> serviceProvider;
	if (FAILED(m_ptr->QueryInterface(IID_PPV_ARGS(serviceProvider.put()))))
		return false;

	wil::com_ptr<IShellBrowser> shellBrowser;
	if (FAILED(serviceProvider->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(shellBrowser.put()))))
		return false;

	::HWND shellBrowserWindow;
	if (FAILED(shellBrowser->GetWindow(&shellBrowserWindow)))
		return false;

	if (shellBrowserWindow == hwnd)
		return true;

	return IsChild(shellBrowserWindow, hwnd);
}
