#pragma once
#include "WebBrowser2.h"

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
