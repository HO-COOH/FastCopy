#pragma once

#include "RobocopyViewModel.g.h"
#include "PropertyChangeHelper.hpp"
#include "ProcessIOUpdater.hpp"
#include <optional>
#include "RobocopyProcess.h"
#include "TaskFile.h"
#include <ppltasks.h>

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
        winrt::hstring SpeedText() { return m_speedText; }
        winrt::hstring SizeText();

        void Start();
        void Pause();
        void Cancel();

        winrt::event_token Finished(winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState> const& handler);
        void Finished(winrt::event_token const& token) noexcept;
#pragma endregion

        void OnUpdateCopySpeed(ProcessIoCounter::IOCounterDiff diff);
    private:
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState>> m_finishEvent;
        std::optional<RobocopyProcess> m_process;
        winrt::hstring m_destination;
        uint64_t m_size{};
        std::optional<TaskFile> m_recordFile;
        std::optional<TaskFile::TaskFileIterator<typename std::vector<std::wstring>::iterator>> m_iter;
        Concurrency::task<void> m_countItemTask;
        winrt::hstring m_speedText;
        int m_finishedFiles{};
        RobocopyArgs getRobocopyArg();
        Status m_status{ Status::Running };
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel, implementation::RobocopyViewModel>
    {
    };
}
