#pragma once

#include "ReadableUnitConverterWinRT.g.h"

namespace winrt::FastCopy::implementation
{
    struct ReadableUnitConverterWinRT : ReadableUnitConverterWinRTT<ReadableUnitConverterWinRT>
    {
        static winrt::hstring ConvertSize(uint64_t size);
        static winrt::hstring ConvertSpeed(double speed);
		static winrt::hstring ConvertPercent(double percentOfOne);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ReadableUnitConverterWinRT : ReadableUnitConverterWinRTT<ReadableUnitConverterWinRT, implementation::ReadableUnitConverterWinRT>
    {
    };
}
