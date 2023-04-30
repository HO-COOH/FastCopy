#include "pch.h"
#include "DoubleToStringConverter.h"
#if __has_include("DoubleToStringConverter.g.cpp")
#include "DoubleToStringConverter.g.cpp"
#endif
#include <sstream>

namespace winrt::FastCopy::implementation
{
	winrt::Windows::Foundation::IInspectable DoubleToStringConverter::Convert(
		winrt::Windows::Foundation::IInspectable const& value, 
		winrt::Windows::UI::Xaml::Interop::TypeName const& targetType, 
		winrt::Windows::Foundation::IInspectable const& parameter, 
		winrt::hstring const& language)
	{
		auto const doubleValue = winrt::unbox_value<double>(value);
		auto const numDigits = std::stoi(winrt::unbox_value_or<winrt::hstring>(parameter, L"0").data());
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(numDigits) << doubleValue;
		return winrt::box_value(ss.str());
	}

	winrt::Windows::Foundation::IInspectable DoubleToStringConverter::ConvertBack(
		winrt::Windows::Foundation::IInspectable const& value, 
		winrt::Windows::UI::Xaml::Interop::TypeName const& targetType, 
		winrt::Windows::Foundation::IInspectable const& parameter, 
		winrt::hstring const& language)
	{
		throw std::exception{};
	}
}
