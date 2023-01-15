#pragma once

#include "XCopyViewModel.g.h"
#include "XCopyFlags.g.h"
#include <winrt/Microsoft.UI.Dispatching.h>
#include "PropertyChangeHelper.hpp"
#include "XCopyCommandGenerator.h"

namespace winrt::FastCopy::implementation
{
    struct XCopyFlags : XCopyFlagsT<XCopyFlags>
    {
    private:
        winrt::hstring m_flag;
        winrt::hstring m_description;
        bool m_enabled = false;
    public:

        winrt::hstring Flag() { return m_flag; }
        winrt::hstring Description() { return m_description; }
        bool Enabled() { return m_enabled; }
        void Enabled(bool value) { m_enabled = value; }

        XCopyFlags(winrt::hstring flag, winrt::hstring description) : m_flag{ flag }, m_description{ description }
        {
        }
    };

    struct XCopyViewModel : XCopyViewModelT<XCopyViewModel>, PropertyChangeHelper<XCopyViewModel>
    {
        XCopyViewModel();
        
        int ProcessCount() { return m_processCount; }
        void ProcessCount(int processCount) { m_processCount = processCount; }

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem> Sources() { return m_sources; }
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem> Preview() { return m_preview; }
        winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> Flags();

        void Execute();

        winrt::hstring Destination() { return m_destination; }
        void Destination(winrt::hstring path);

        Mode Mode() { return m_mode; }
        void Mode(FastCopy::Mode mode);
        bool IsCustomEnabled();

        bool IsExecutable() 
        { 
            auto result = !m_destination.empty() && m_sources.Size() != 0;
            return result;
        }
    private:
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem> m_sources{ winrt::single_threaded_observable_vector<winrt::FastCopy::ExplorerItem>() };
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::ExplorerItem> m_preview{ winrt::single_threaded_observable_vector<winrt::FastCopy::ExplorerItem>() };

        winrt::hstring m_destination;
        FastCopy::Mode m_mode = Mode::Copy;

        void execute();
        void clearAll();

        std::vector<Task> m_tasks;
        decltype(m_tasks.begin()) m_taskIter;

        std::vector<HANDLE> m_processHandles;

        int m_processCount = std::thread::hardware_concurrency();
       
        HANDLE executeSingleTask();

        winrt::Microsoft::UI::Dispatching::DispatcherQueue m_dispatcher{ nullptr };

        std::string m_destinationA;

        void deleteSource();
        void refreshPreview();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct XCopyViewModel : XCopyViewModelT<XCopyViewModel, implementation::XCopyViewModel>
    {
    };

    struct XCopyFlags : XCopyFlagsT<XCopyFlags, implementation::XCopyFlags>
    {

    };
}
