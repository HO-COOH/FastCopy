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
#include "RobocopyArgs.h"

namespace winrt::FastCopy::implementation
{

	winrt::hstring RobocopyViewModel::RecordFile()
	{
		throw std::logic_error{"Dead code"};
	}
	void RobocopyViewModel::RecordFile(winrt::hstring value)
	{
		m_recordFile.emplace(value);

		m_countItemTask = concurrency::create_task([this]
		{
			m_recordFile->GetNumFiles();
			m_iter = m_recordFile->begin();
			Global::UIThread.TryEnqueue([this] {
				raisePropertyChange(L"ItemCount");
			});
			Start();
		});
	}
	winrt::hstring RobocopyViewModel::OperationString()
	{
		if (m_recordFile)
			return L" ";
		switch (std::filesystem::path{ m_recordFile->data()}.filename().wstring()[0])
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
		return m_recordFile.has_value() ? m_recordFile->GetNumFiles() : 0;
	}
	winrt::hstring RobocopyViewModel::Source()
	{
		if (m_recordFile && m_iter >= m_recordFile->begin() && m_iter < m_recordFile->end())
			return std::filesystem::path{ *m_iter }.filename().wstring().data();
		return L"---";
	}
	winrt::hstring RobocopyViewModel::Destination()
	{
		return m_destination.empty() ? L"---" : std::filesystem::path{ m_destination.data()}.filename().wstring().data();
	}
	void RobocopyViewModel::Destination(winrt::hstring value)
	{
		MessageBox(NULL, value.data(), L"", 0);
		m_destination = value;
		raisePropertyChange(L"Destination");
	}
	double RobocopyViewModel::Percent()
	{
		if (!m_recordFile.has_value())
			return 0;

		return min(100.0, static_cast<float>(m_finishedFiles) / m_recordFile->GetNumFiles() * 100.0);
	}
	void RobocopyViewModel::Start()
	{
		ProcessIOUpdater::Start(std::chrono::milliseconds{ 300 });

		concurrency::create_task([this]
		{
			m_countItemTask.get();

			while (m_iter != m_recordFile->end())
			{
				Global::UIThread.TryEnqueue([this] {raisePropertyChange(L"Source"); });
				m_process.emplace(getRobocopyArg());
				SetHandle(m_process->Handle());
				auto const ret = m_process->WaitForExit();
				m_finishedFiles += m_recordFile->GetNumFiles(m_recordFile->IndexOf(m_iter));
				Global::UIThread.TryEnqueue([this] 
				{ 
					raisePropertyChange(L"Percent"); 
				});
				++m_iter;
			}
		});
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
	RobocopyArgs RobocopyViewModel::getRobocopyArg()
	{
		RobocopyArgs args;
		args.destinationDir = m_destination;
		if (std::filesystem::is_directory(*m_iter))
		{
			args.sourceDir = *m_iter;
			args.destinationDir += L"\\" + std::filesystem::path{ *m_iter }.filename().wstring();
		}
		else
		{
			std::filesystem::path path{ *m_iter };
			args.sourceDir = path.parent_path().wstring();
			args.files.push_back(path.filename().wstring());
		}
		return args;
	}
}
