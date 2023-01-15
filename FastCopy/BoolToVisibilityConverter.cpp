#include "pch.h"
#include "BoolToVisibilityConverter.h"
#if __has_include("BoolToVisibilityConverter.g.cpp")
#include "BoolToVisibilityConverter.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
    winrt::Windows::Foundation::IInspectable BoolToVisibilityConverter::Convert(
        winrt::Windows::Foundation::IInspectable const& value,
        [[maybe_unused]] winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
        [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& parameter,
        [[maybe_unused]] winrt::hstring const& language)
    {
        return winrt::box_value(winrt::unbox_value<bool>(value) ?
            winrt::Microsoft::UI::Xaml::Visibility::Visible :
            winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
    }

    winrt::Windows::Foundation::IInspectable BoolToVisibilityConverter::ConvertBack(
        [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& value,
        [[maybe_unused]] winrt::Windows::UI::Xaml::Interop::TypeName const& targetType,
        [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& parameter,
        [[maybe_unused]] winrt::hstring const& language)
    {
        throw std::exception{ "Not implemented" };
    }
}
