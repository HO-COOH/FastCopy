#include "pch.h"
#include "ReadableUnitConverterWinRT.h"
#if __has_include("ReadableUnitConverterWinRT.g.cpp")
#include "ReadableUnitConverterWinRT.g.cpp"
#endif
#include "ReadableUnitConverter.h"

namespace winrt::FastCopy::implementation
{
	static auto getNullFileTime()
	{
		return winrt::clock::from_FILETIME({});
	}

	winrt::hstring ReadableUnitConverterWinRT::ConvertSize(uint64_t size)
	{
		return winrt::hstring{ size == 0? L"---" : ReadableUnitConverter::Size::ToString<wchar_t>(size)};
	}

	winrt::hstring ReadableUnitConverterWinRT::ConvertSpeed(double speed)
	{
		return winrt::hstring{ ReadableUnitConverter::Speed::ToString<wchar_t>(speed) };
	}

	winrt::hstring ReadableUnitConverterWinRT::ConvertPercent(double percentOfOne)
	{
		auto const doubleValue = percentOfOne * 100.0;
		auto const numDigits = 2;
		std::wstringstream ss;
		ss << std::fixed << std::setprecision(numDigits) << doubleValue;
		return winrt::hstring{ ss.str() + L" %" };
	}
	winrt::hstring ReadableUnitConverterWinRT::ConvertDateTime(winrt::Windows::Foundation::DateTime dateTime)
	{
		if (static auto nullFileTime = getNullFileTime(); dateTime == nullFileTime)
			return L"---";

		auto p2 = std::chrono::clock_cast<std::chrono::system_clock>(dateTime);
		auto result = (std::wstringstream{} << p2).str();
		result = result.substr(0, result.rfind(L"."));
		return winrt::hstring{ std::move(result) };
	}
}
