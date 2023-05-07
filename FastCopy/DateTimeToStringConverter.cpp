#include "pch.h"
#include "DateTimeToStringConverter.h"
#if __has_include("DateTimeToStringConverter.g.cpp")
#include "DateTimeToStringConverter.g.cpp"
#endif
#include <sstream>

namespace winrt::FastCopy::implementation
{
    winrt::Windows::Foundation::IInspectable DateTimeToStringConverter::Convert(
        winrt::Windows::Foundation::IInspectable const& value, 
        winrt::Windows::UI::Xaml::Interop::TypeName const& targetType, 
        winrt::Windows::Foundation::IInspectable const& parameter,
        winrt::hstring const& language)
    {
        auto const p = winrt::unbox_value<winrt::Windows::Foundation::DateTime>(value);
        auto p2 = std::chrono::clock_cast<std::chrono::system_clock>(p);
        auto result = (std::wstringstream{} << p2).str();
        result = result.substr(0, result.rfind(L"."));
        return winrt::box_value(result);
    }

    winrt::Windows::Foundation::IInspectable DateTimeToStringConverter::ConvertBack(
        winrt::Windows::Foundation::IInspectable const& , 
        winrt::Windows::UI::Xaml::Interop::TypeName const& , 
        winrt::Windows::Foundation::IInspectable const& , 
        winrt::hstring const& )
    {
        throw std::exception{ "Not implemented" };
    }
}
