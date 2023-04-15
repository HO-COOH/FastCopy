#include "pch.h"
#include "RobocopyViewModel.h"
#if __has_include("RobocopyViewModel.g.cpp")
#include "RobocopyViewModel.g.cpp"
#endif
#include <ppltasks.h>
#include <fstream>
#include <filesystem>

namespace winrt::FastCopy::implementation
{
	winrt::hstring RobocopyViewModel::RecordFile()
	{
		return winrt::hstring();
	}
	void RobocopyViewModel::RecordFile(winrt::hstring value)
	{
		m_recordFile = value;
		concurrency::create_task(
			[this]()
			{
				std::ifstream fs{ m_recordFile.data()};

			}
		);
		Start();
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
		return 20;
	}
	winrt::hstring RobocopyViewModel::Source()
	{
		return L"Desktop";
	}
	winrt::hstring RobocopyViewModel::Destination()
	{
		return L"Documents";
	}
	double RobocopyViewModel::Percent()
	{
		return 51;
	}
	void RobocopyViewModel::Start()
	{
		ProcessIOUpdater::Start(std::chrono::seconds{ 1 });
	}
	void RobocopyViewModel::Pause()
	{
	}
	void RobocopyViewModel::Cancel()
	{
	}
	void RobocopyViewModel::OnUpdate(ProcessIoCounter::IOCounterDiff diff)
	{
		OutputDebugString(std::format(L"{}\n", diff.read).data());
	}
}
