#pragma once

#include "RobocopyViewModel.g.h"
#include "PropertyChangeHelper.hpp"
#include "ProcessIOUpdater.hpp"
#include <optional>
#include <chrono>
#include "RobocopyArgs.h"
#include "RobocopyJobFile.h"
#include "TaskFile.h"
#include <wil/coroutine.h>
#include <atomic>
#include <filesystem>
#include <mutex>
#include "RobocopyProcessPool.h"

namespace winrt::FastCopy::implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel>, PropertyChangeHelper<RobocopyViewModel>, ProcessIOUpdater<RobocopyViewModel>
    {
        RobocopyViewModel(winrt::hstring const& recordFile, winrt::hstring const& destination);
        ~RobocopyViewModel();

#pragma region ICopyBase
        winrt::hstring OperationString();
        int ItemCount();
        int FinishedItemCount();
        winrt::hstring Source();
        winrt::hstring Destination();
        double Percent();

        void Start();
        void TogglePause();
        void Cancel();
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
        double Speed() { return m_bytesPerSec; }

		FastCopy::TaskbarState State();

        winrt::hstring ErrorText();

        TaskFile m_recordFile;
        winrt::hstring const m_destination;
    private:
        struct RobocopyWorkItem
        {
            RobocopyArgsBuilder args;
            std::vector<std::wstring> jobFiles;
            std::filesystem::path sourceRootDir;
            std::filesystem::path destRootDir;
            std::filesystem::path displaySource;
            int32_t itemFileCount{};
            bool deleteDestRoot{};
        };

        double m_bytesPerSec{};

        std::atomic_bool m_hasDuplicates{ false };
        uint32_t m_useSourceCount{};
        uint32_t m_useDestinationCount{};
        RobocopyProcessPool m_processPool;
        std::atomic_bool m_robocopyStageFinished{ false };
        std::atomic_bool m_duplicatesFinished{ false };
        FastCopy::TaskbarState m_state{ FastCopy::TaskbarState::Indeterminate };

        std::atomic_uint64_t m_copiedBytes{};
        TaskFile::TaskCount m_taskCount{};
        std::mutex m_sourceMutex;
        winrt::hstring m_source{ L"---" };

        std::optional<std::vector<winrt::FastCopy::FileCompareViewModel>> m_duplicateFileTask;
        std::optional<std::vector<winrt::FastCopy::FileCompareViewModel>::iterator> m_duplicateFileTaskIter;

        wil::task<TaskFile::TaskCount> m_countItemTask;
        std::atomic_int m_finishedFiles{};
        std::mutex m_errorsMutex;
        std::wstring m_errorText; //robocopy ERROR lines joined for display; bound to CopyDialogWindow's ErrorText TextBlock
        std::atomic_bool m_allFinished{ false }; //guards onAllFinished() so it runs exactly once
        std::atomic<std::chrono::steady_clock::time_point> m_lastProgressUpdate{}; //throttle: when the last progress UI update was enqueued

        wil::task<TaskFile::TaskCount> countItemsAsync();
        winrt::fire_and_forget runCopyLoopAsync();
        std::vector<RobocopyWorkItem> buildWorkItems();
        RobocopyWorkItem makeDirectoryWorkItem(std::filesystem::path const& sourcePath, int32_t itemFileCount);
        RobocopyWorkItem makeFileBatchWorkItem(std::filesystem::path const& parentDir, std::vector<std::wstring>&& fileNames, bool useJobFile);
        void launchWorkItem(size_t currentIndex, RobocopyWorkItem&& item);
        void completeRobocopyStage();
        void updateCurrentSource(std::wstring_view path);
        void updateCurrentSource(std::string_view utf8Path);

        /**
         * @brief Called after normal robocopy stages finished 
         */
        void onNormalRobocopyFinished();

        void onAllFinished();

        void appendError(std::string_view error);
        void abortWithError(std::string_view message);

        void raiseProgressChange();
        winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> m_duplicateFiles = winrt::single_threaded_observable_vector<winrt::FastCopy::FileCompareViewModel>();
    };
}
