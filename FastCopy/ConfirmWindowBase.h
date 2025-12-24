#pragma once
namespace winrt
{
	namespace Windows::Foundation
	{
		struct IInspectable;
	}

	namespace Microsoft::UI::Xaml
	{
		struct RoutedEventArgs;
	}
}

template<typename Derived>
struct ConfirmWindowBase
{
	bool Result{};

	void YesButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		Result = true;
		static_cast<Derived*>(this)->Close();
	}

	void NoButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		Result = false;
		static_cast<Derived*>(this)->Close();
	}
};
