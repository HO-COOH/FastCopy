#include "pch.h"
#include "ReadableSizeConverter.h"
#if __has_include("ReadableSizeConverter.g.cpp")
#include "ReadableSizeConverter.g.cpp"
#endif
#include "ReadableUnitConverter.h"

namespace winrt::FastCopy::implementation
{
	winrt::Windows::Foundation::IInspectable ReadableSizeConverter::Convert(
		winrt::Windows::Foundation::IInspectable const& value,
		winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
		winrt::Windows::Foundation::IInspectable const& parameter,
		winrt::hstring const& language)
	{
		return winrt::box_value(ReadableUnitConverter::Size::ToString<wchar_t>(winrt::unbox_value<uint64_t>(value)));
	}

	winrt::Windows::Foundation::IInspectable ReadableSizeConverter::ConvertBack(
		winrt::Windows::Foundation::IInspectable const& value,
		winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
		winrt::Windows::Foundation::IInspectable const& parameter,
		winrt::hstring const& language)
	{
		throw std::exception{};
	}
}
