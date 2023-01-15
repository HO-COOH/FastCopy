#pragma once

#include "ExplorerItemTemplateSelector.g.h"

namespace winrt::FastCopy::implementation
{
    struct ExplorerItemTemplateSelector : ExplorerItemTemplateSelectorT<ExplorerItemTemplateSelector>
    {
    private:
        winrt::Microsoft::UI::Xaml::DataTemplate m_folderTemplate{ nullptr };
        winrt::Microsoft::UI::Xaml::DataTemplate m_fileTemplate{ nullptr };
    public:
        ExplorerItemTemplateSelector() = default;
        
        winrt::Microsoft::UI::Xaml::DataTemplate FolderTemplate() { return m_folderTemplate; }
        void FolderTemplate(winrt::Microsoft::UI::Xaml::DataTemplate folderTemplate) { m_folderTemplate = folderTemplate; }

        winrt::Microsoft::UI::Xaml::DataTemplate FileTemplate() { return m_fileTemplate; }
        void FileTemplate(winrt::Microsoft::UI::Xaml::DataTemplate fileTemplate) { m_fileTemplate = fileTemplate; }

        winrt::Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(winrt::Windows::Foundation::IInspectable const& item);
        winrt::Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(winrt::Windows::Foundation::IInspectable const& item, winrt::Microsoft::UI::Xaml::DependencyObject object);
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ExplorerItemTemplateSelector : ExplorerItemTemplateSelectorT<ExplorerItemTemplateSelector, implementation::ExplorerItemTemplateSelector>
    {
    };
}
