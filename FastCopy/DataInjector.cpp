#include "pch.h"
#include "DataInjector.h"
#if __has_include("DataInjector.g.cpp")
#include "DataInjector.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
	Microsoft::UI::Xaml::DependencyProperty DataInjector::s_dataProperty = Microsoft::UI::Xaml::DependencyProperty::RegisterAttached(
		L"Data",
		winrt::xaml_typename<winrt::Windows::Foundation::IInspectable>(),
		winrt::xaml_typename<winrt::FastCopy::DataInjector>(),
		nullptr
	);


	Microsoft::UI::Xaml::DependencyProperty DataInjector::DataProperty()
	{
		return s_dataProperty;
	}

	winrt::Windows::Foundation::IInspectable DataInjector::GetData(Microsoft::UI::Xaml::DependencyObject target)
	{
		return target.GetValue(s_dataProperty);
	}

	void DataInjector::SetData(Microsoft::UI::Xaml::DependencyObject target, winrt::Windows::Foundation::IInspectable data)
	{
		target.SetValue(s_dataProperty, data);
	}

}
