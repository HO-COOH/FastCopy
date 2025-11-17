#pragma once
#include <wil/com.h>
#include <ExDisp.h>

class WebBrowser2
{
	wil::com_ptr<IWebBrowser2> m_ptr;
public:
	WebBrowser2(wil::com_ptr<IWebBrowser2>&& ptr);

	HWND HWND();

	//this will be the folder name, not the full path
	wil::unique_bstr LocationName();

	//this will be a file URL, like: file:///c/
	wil::unique_bstr LocationURL();

	bool Visible();
};
