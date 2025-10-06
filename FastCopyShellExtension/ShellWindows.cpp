#include "ShellWindows.h"
#include <wil/result_macros.h>
#include <atlcomcli.h>
#include <cassert>

long ShellWindows::Count()
{
	long value{};
	THROW_IF_FAILED(m_ptr->get_Count(&value));
	return value;
}

WebBrowser2 ShellWindows::Item(VARIANT index)
{
	wil::com_ptr<IDispatch> value;
	THROW_IF_FAILED(m_ptr->Item(index, value.put()));
	return WebBrowser2{ value.query<IWebBrowser2>() };
}

std::optional<WebBrowser2> ShellWindows::GetForegroundExplorer()
{
    std::optional<WebBrowser2> ret;

    auto const hwnd = GetForegroundWindow();
    ShellWindows shellWindows;

    auto const count = shellWindows.Count();
    for (long i = 0; i < count; ++i)
    {
        CComVariant vi{ i };
        auto item = shellWindows.Item(vi);
        if (item.HWND() == hwnd)
        {
            ret.emplace(std::move(item));
            break;
        }
    }

    return ret;
}


