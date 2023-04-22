#pragma once

#include "RobocopyViewModel.g.h"
#include "PropertyChangeHelper.hpp"
#include "ProcessIOUpdater.hpp"
#include <optional>
#include "RobocopyProcess.h"

namespace winrt::FastCopy::implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel>, PropertyChangeHelper<RobocopyViewModel>, ProcessIOUpdater<RobocopyViewModel>
    {
        RobocopyViewModel() = default;

        winrt::hstring RecordFile();
        void RecordFile(winrt::hstring value);

        winrt::hstring OperationString();
        int ItemCount();
        winrt::hstring Source();
        winrt::hstring Destination();
        void Destination(winrt::hstring value);
        double Percent();

        void Start();
        void Pause();
        void Cancel();

        void OnUpdateCopySpeed(ProcessIoCounter::IOCounterDiff diff);

        winrt::hstring SpeedText() { return m_speedText; }
        winrt::hstring SizeText();

        winrt::event_token Finished(winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState> const& handler);
        void Finished(winrt::event_token const& token) noexcept;
    private:
        winrt::event<winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState>> m_finishEvent;
        std::optional<RobocopyProcess> m_process;
        winrt::hstring m_destination;
        uint64_t m_size{};
        int m_itemCount{};
        winrt::hstring m_recordFile;
        winrt::hstring m_speedText;
        double m_percent{};
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct RobocopyViewModel : RobocopyViewModelT<RobocopyViewModel, implementation::RobocopyViewModel>
    {
    };
}
