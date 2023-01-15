#include "pch.h"
#include "NumberToStringConverter.h"
#if __has_include("NumberToStringConverter.g.cpp")
#include "NumberToStringConverter.g.cpp"
#endif
#include <format>

namespace winrt::FastCopy::implementation
{
	constexpr static auto ToKB = 1024ull;
	constexpr static auto ToMB = 1024 * 1024ull;
	constexpr static auto ToGB = 1024 * 1024 * 1024ull;
	constexpr static auto B = L" B";
	constexpr static auto KB = L" KB";
	constexpr static auto MB = L" MB";
	constexpr static auto GB = L" GB";

	static auto ConvertUnit(uint64_t value)
	{
		auto const valueDouble = static_cast<double>(value);
		if (valueDouble / static_cast<double>(ToKB) < 1.0)
			return std::make_pair(valueDouble, B);
		else if (valueDouble / static_cast<double>(ToMB) < 1.0)
			return std::make_pair(valueDouble / static_cast<double>(ToKB), KB);
		else if (valueDouble / static_cast<double>(ToGB) < 1.0)
			return std::make_pair(valueDouble / static_cast<double>(ToMB), MB);
		else
			return std::make_pair(valueDouble / static_cast<double>(ToGB), GB);
	}

	winrt::Windows::Foundation::IInspectable NumberToStringConverter::Convert(
		winrt::Windows::Foundation::IInspectable const& value,
		[[maybe_unused]] winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
		[[maybe_unused]] winrt::Windows::Foundation::IInspectable const& parameter,
		[[maybe_unused]] winrt::hstring const& language)
	{
		auto const byteValue = winrt::unbox_value<uint64_t>(value);
		auto const [converted, unit] = ConvertUnit(byteValue);
		return winrt::box_value(std::format(L"{:.2f} {}", converted, unit));
	}

	winrt::Windows::Foundation::IInspectable NumberToStringConverter::ConvertBack(
		[[maybe_unused]] winrt::Windows::Foundation::IInspectable const& value,
		[[maybe_unused]] winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
		[[maybe_unused]] winrt::Windows::Foundation::IInspectable const& parameter,
		[[maybe_unused]] winrt::hstring const& language)
	{
		throw std::exception{};
	}
}
