#include "pch.h"
#include "ResourceHelper.h"
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <format>

winrt::hstring GetStringResource(std::wstring_view key)
{
    static winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager manager;
    return manager.MainResourceMap().GetValue(std::format(L"Resources/{}", key)).ValueAsString();
}

winrt::hstring ToCapital(winrt::hstring word)
{
    assert(!word.empty());
    std::wstring ret{word};
    ret[0] = towupper(ret[0]);
    return winrt::hstring{std::move(ret)};
}
