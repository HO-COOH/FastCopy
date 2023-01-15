#include "pch.h"
#include "VectorToBoolConverter.h"
#if __has_include("VectorToBoolConverter.g.cpp")
#include "VectorToBoolConverter.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
    winrt::Windows::Foundation::IInspectable VectorToBoolConverter::Convert(
        winrt::Windows::Foundation::IInspectable const& value,
        [[maybe_unused]] winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
        [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& parameter,
        [[maybe_unused]] winrt::hstring const& language)
    {
        auto vector = value.try_as<winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem>>();
        if (!vector)
            return winrt::box_value(false);
        return winrt::box_value(vector.Size()? true : false);
    }

    winrt::Windows::Foundation::IInspectable VectorToBoolConverter::ConvertBack(
        [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& value,
        [[maybe_unused]] winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
        [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& parameter,
        [[maybe_unused]] winrt::hstring const& language)
    {
        throw std::exception{ "Not implemented" };
    }
}
