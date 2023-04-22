#include "pch.h"
#include "RobocopyViewModel.h"
#if __has_include("RobocopyViewModel.g.cpp")
#include "RobocopyViewModel.g.cpp"
#endif
#include <ppltasks.h>
#include <fstream>
#include <filesystem>
#include "ReadableUnitConverter.h"
#include "Global.h"
#include "FileStats.h"

namespace winrt::FastCopy::implementation
{
	static RobocopyArgs GetRobocopyArg(winrt::hstring const& filePath, winrt::hstring const& destination)
	{
		std::wifstream file{ filePath.data() };
		std::wstring line;
		std::getline(file, line);

		//switch (std::filesystem::path{ filePath.data() }.filename().wstring()[0])
		//{
		//	case L'C': return std::format(LR"("{}")",1);
		//	case L'M': return L"Moving ";
		//	case L'D': return L"Deleting ";
		//	default:
		//		throw std::runtime_error{ "Invalid operation" };
		//}

		RobocopyArgs args;
		args.sourceDir = line;
		args.destinationDir = destination;
		return args;
	}

	winrt::hstring RobocopyViewModel::RecordFile()
	{
		return winrt::hstring();
	}
	void RobocopyViewModel::RecordFile(winrt::hstring value)
	{
		m_recordFile = value;
		//concurrency::create_task(
		//	[this]()
		//	{
		//		std::ifstream fs{ m_recordFile.data()};

		//	}
		//);

		Start();
		concurrency::create_task([this]
		{
			m_itemCount = FileStats::GetNumFiles(m_recordFile);
			Global::UIThread.TryEnqueue([this] {
				raisePropertyChange(L"ItemCount");
			});
		});
	}
	winrt::hstring RobocopyViewModel::OperationString()
	{
		if (m_recordFile.empty())
			return L" ";
		switch (std::filesystem::path{ m_recordFile.data()}.filename().wstring()[0])
		{
			case L'C': return L"Copying ";
			case L'M': return L"Moving ";
			case L'D': return L"Deleting ";
			default:
				throw std::runtime_error{ "Invalid operation" };
		}
	}
	int RobocopyViewModel::ItemCount()
	{
		return m_itemCount;
	}
	winrt::hstring RobocopyViewModel::Source()
	{
		return L"Desktop";
	}
	winrt::hstring RobocopyViewModel::Destination()
	{
		return L"Documents";
	}
	void RobocopyViewModel::Destination(winrt::hstring value)
	{
		MessageBox(NULL, value.data(), L"", 0);
		m_destination = value;
	}
	double RobocopyViewModel::Percent()
	{
		return m_percent;
	}
	void RobocopyViewModel::Start()
	{
		SetHandle(OpenProcess(PROCESS_QUERY_INFORMATION, false, 30552));
		ProcessIOUpdater::Start(std::chrono::seconds{ 1 });
		winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
			[this](auto timer)
			{
				if (m_percent < 99)
				{
					m_percent += 1.0;
					Global::UIThread.TryEnqueue([this] {
						raisePropertyChange(L"Percent");
					});
				}
				else
				{
					timer.Cancel();
					m_finishEvent(*this, FinishState::Success);
				}
			}, std::chrono::milliseconds{30}
		);


		m_process.emplace(GetRobocopyArg(m_recordFile, m_destination));
		concurrency::create_task(
			[this]
			{
				auto const ret = m_process->WaitForExit();
				m_percent = 100.0;
				Global::UIThread.TryEnqueue([this] { raisePropertyChange(L"Percent"); });
			}
		);
	}
	void RobocopyViewModel::Pause()
	{
	}
	void RobocopyViewModel::Cancel()
	{
	}
	void RobocopyViewModel::OnUpdateCopySpeed(ProcessIoCounter::IOCounterDiff diff)
	{
		m_speedText = ReadableUnitConverter::Speed::ToString<wchar_t>(diff.read, diff.duration);
		Global::UIThread.TryEnqueue([this] {
			raisePropertyChange(L"SpeedText");
		});
	}
	winrt::hstring RobocopyViewModel::SizeText()
	{
		return m_size == 0 ?
			L"---" :
			ReadableUnitConverter::Size::ToString<wchar_t>(m_size).data();
	}
	winrt::event_token RobocopyViewModel::Finished(winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState> const& handler)
	{
		return m_finishEvent.add(handler);
	}
	void RobocopyViewModel::Finished(winrt::event_token const& token) noexcept
	{
		m_finishEvent.remove(token);
	}
}
