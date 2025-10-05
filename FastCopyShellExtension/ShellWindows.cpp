#include "ShellWindows.h"
#include <wil/result_macros.h>
#include <atlcomcli.h>

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

std::vector<WebBrowser2> ShellWindows::GetCurrentOpenedExplorerFolders()
{
    ShellWindows shellWindows;

    auto const count = shellWindows.Count();
    std::vector<WebBrowser2> result;
    result.reserve(count);
    for (long i = 0; i < count; ++i)
    {
        CComVariant vi{ i };
        result.emplace_back(shellWindows.Item(vi));
    }
    return result;
}


