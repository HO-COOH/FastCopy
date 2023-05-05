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
        auto const t = winrt::clock::to_time_t(p);
        return winrt::box_value((std::wstringstream{} << t).str());
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
