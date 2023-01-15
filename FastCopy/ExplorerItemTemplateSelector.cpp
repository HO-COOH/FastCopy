#include "pch.h"
#include "ExplorerItemTemplateSelector.h"
#if __has_include("ExplorerItemTemplateSelector.g.cpp")
#include "ExplorerItemTemplateSelector.g.cpp"
#endif

namespace winrt::FastCopy::implementation
{
	winrt::Microsoft::UI::Xaml::DataTemplate ExplorerItemTemplateSelector::SelectTemplateCore(winrt::Windows::Foundation::IInspectable const& item)
	{
		switch (item.as<winrt::FastCopy::ExplorerItem>().Type())
		{
			case		winrt::FastCopy::ExplorerItemType::File: return m_fileTemplate;
			default:	return m_folderTemplate;
		}
	}
	winrt::Microsoft::UI::Xaml::DataTemplate ExplorerItemTemplateSelector::SelectTemplateCore(winrt::Windows::Foundation::IInspectable const& item, winrt::Microsoft::UI::Xaml::DependencyObject)
	{
		return this->SelectTemplateCore(item);
	}
}
