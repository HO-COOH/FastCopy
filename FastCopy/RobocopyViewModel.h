#pragma once

#include "RobocopyViewModel.g.h"
#include "PropertyChangeHelper.hpp"
#include "ProcessIOUpdater.hpp"
#include <optional>
#include "RobocopyProcess.h"
#include "TaskFile.h"
#include <ppltasks.h>
#include "FileCompareViewModel.g.h"
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::FastCopy::implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel>, PropertyChangeHelper<RobocopyViewModel>, ProcessIOUpdater<RobocopyViewModel>
    {
        RobocopyViewModel()
        {
            m_duplicateFiles.Append({LR"(E:\android-studio-2021.2.1.15-windows_2.exe)", LR"(E:\527.56-desktop-win10-win11-64bit-international-dch-whql.exe)"});
        }

        winrt::hstring RecordFile();
        void RecordFile(winrt::hstring value);

        winrt::Windows::Foundation::Uri DestinationUri();
#pragma region ICopyBase
        winrt::hstring OperationString();
        int ItemCount();
        int FinishedItemCount();
        winrt::hstring Source();
        winrt::hstring Destination();
        void Destination(winrt::hstring value);
        double Percent();
        winrt::hstring SpeedText() { return m_speedText; }
        winrt::hstring SizeText();

        void Start();
        void Pause();
        void Cancel();

        winrt::event_token Finished(winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState> const& handler);
        void Finished(winrt::event_token const& token) noexcept;
#pragma endregion

        void OnUpdateCopySpeed(ProcessIoCounter::IOCounterDiff diff);

        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> DuplicateFiles() { return m_duplicateFiles; }
        winrt::Windows::Foundation::IReference<bool> UseSource();
        void UseSource(winrt::Windows::Foundation::IReference<bool> value);
        winrt::Windows::Foundation::IReference<bool> UseDestination();
        void UseDestination(winrt::Windows::Foundation::IReference<bool> value);

        void AddSource() { ++m_useSourceCount; raisePropertyChange(L"UseSource"); }
        void RemoveSource() { --m_useSourceCount; raisePropertyChange(L"UseSource"); }
        void AddDestination() { ++m_useDestinationCount; raisePropertyChange(L"UseDestination"); }
        void RemoveDestination() { --m_useDestinationCount; raisePropertyChange(L"UseDestination"); }
    private:
        int m_useSourceCount{};
        int m_useDestinationCount{};
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState>> m_finishEvent;
        std::optional<RobocopyProcess> m_process;
        winrt::hstring m_destination;
        uint64_t m_size{};
        std::optional<TaskFile> m_recordFile;
        mutable std::optional<TaskFile::TaskFileIterator<typename std::vector<std::wstring>::iterator>> m_iter;
        Concurrency::task<void> m_countItemTask;
        winrt::hstring m_speedText;
        int m_finishedFiles{};
        RobocopyArgs getRobocopyArg();
        Status m_status{ Status::Running };

        /**
         * @brief When there are files with same filename, robocopy cannot change the filename that it is copied to
         * returns false in such cases
         */
        bool canUseRobocopy() const;
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> m_duplicateFiles = winrt::single_threaded_observable_vector<winrt::FastCopy::FileCompareViewModel>();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel, implementation::RobocopyViewModel>
    {
    };
}
