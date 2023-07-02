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
        RobocopyViewModel() = default;

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
        winrt::hstring SpeedText();
        winrt::hstring SizeText();

        winrt::Windows::Foundation::IAsyncAction Start();
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

        void ConfirmDuplicates();
        bool CanContinue() { return m_canContinue; }
        double Speed() { return m_bytesPerSec; }
        winrt::Windows::Foundation::IAsyncOperation<uint64_t> GetTotalSize();

        ~RobocopyViewModel();
    private:
        double m_bytesPerSec{};
        double m_lastPercent{};
        bool m_canContinue = false;
        bool m_hasDuplicates = false;
        int m_useSourceCount{};
        int m_useDestinationCount{};
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState>> m_finishEvent;
        std::optional<RobocopyProcess> m_process;
        winrt::hstring m_destination;
        uint64_t m_copiedBytes{};
        uint64_t m_size{};

        std::optional<TaskFile> m_recordFile;
        uint64_t m_totalSize{};
        mutable std::optional<TaskFile::TaskFileIterator<typename std::vector<std::wstring>::iterator>> m_iter;
        TaskFile::TaskFileIterator<typename std::vector<std::wstring>::iterator> m_recordFileBegin;
        TaskFile::TaskFileIterator<typename std::vector<std::wstring>::iterator> m_recordFileEnd;

        std::optional<std::vector<winrt::FastCopy::FileCompareViewModel>> m_duplicateFileTask;
        std::optional<std::vector<winrt::FastCopy::FileCompareViewModel>::iterator> m_duplicateFileTaskIter;
        std::atomic_bool m_hasConfirmed = false;

        Concurrency::task<void> m_countItemTask;
        int m_finishedFiles{};
        RobocopyArgs getRobocopyArg();
        Status m_status{ Status::Running };

        /**
         * @brief When there are files with same filename, robocopy cannot change the filename that it is copied to
         * returns false in such cases
         */
        bool canUseRobocopy() const;

        /**
         * @brief If the source and destination are under the same disk partition, use IFileOperation APIs takes literally no time
         * @return True if source and destination are under the same disk partition
         */
        bool canUseShellCopy() const;

        /**
         * @brief Called after normal robocopy stages finished 
         */
        void onNormalRobocopyFinished();

        /**
         * @brief Called after fallback copy stages finished
         */
        void onFallbackFinished();

        void onAllFinished();

        winrt::hstring finishedOperationString();

        void raiseProgressChange();
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> m_duplicateFiles = winrt::single_threaded_observable_vector<winrt::FastCopy::FileCompareViewModel>();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel, implementation::RobocopyViewModel>
    {
    };
}
