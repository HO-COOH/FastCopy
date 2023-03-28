#pragma once
#include "PropertyChangeHelper.hpp"
#include "ExplorerItem.g.h"

namespace winrt::FastCopy::implementation
{
    struct ExplorerItem : ExplorerItemT<ExplorerItem>, PropertyChangeHelper<ExplorerItem>
    {
    private:
        ExplorerItemType m_type;
        bool m_isFinished = false;
        bool m_isFailed = false;
        uint64_t m_size{};
        winrt::hstring m_name;
        int m_level = -1;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem> m_children = winrt::single_threaded_observable_vector<winrt::FastCopy::ExplorerItem>();

        void recurseImpl();
        winrt::fire_and_forget calculateSize(winrt::Windows::Storage::IStorageItem item);
        winrt::fire_and_forget calculateSize(winrt::hstring path);
    public:
        ExplorerItem(winrt::hstring name, ExplorerItemType type);
        ExplorerItem(winrt::Windows::Storage::StorageFile file) : m_type{ FastCopy::ExplorerItemType::File }, m_name{ file.Path() } {}
        ExplorerItem(winrt::Windows::Storage::StorageFolder folder, bool recurse, int level);

        ExplorerItemType Type() { return m_type; }
        void Type(ExplorerItemType type);

        winrt::hstring Name() { return m_name; }
        void Name(winrt::hstring name) { m_name = name; }

        winrt::hstring FileName()
        {
            std::wstring_view temp = m_name;
            return winrt::hstring{ temp.substr(temp.rfind(L"\\") + 1) };
        }

        bool IsFinished() 
        { 
            return m_isFinished; 
        }
        void IsFinished(bool finished) 
        { 
            if (m_isFinished != finished)
            {
                m_isFinished = finished;
                raisePropertyChange(L"IsFinished");
            }
        }

        uint64_t Size() { return m_size; }

        bool IsFailed() { return m_isFailed; }
        void IsFailed(bool isFailed);

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem> Children() { return m_children; }

        winrt::Windows::Foundation::IAsyncAction OpenLocation();

        //status
        uint32_t SecondsRemain() { return 0; }
        uint64_t SizeRemain() { return 0; }
        double Progress() { return 0.5; }
        winrt::hstring CurrentFile() { return L"test.file"; }
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ExplorerItem : ExplorerItemT<ExplorerItem, implementation::ExplorerItem>
    {
    };
}
