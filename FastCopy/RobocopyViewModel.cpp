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
#include "Taskbar.h"
#include "Notification.h"
#include "Fallback.h"

namespace winrt::FastCopy::implementation
{

	winrt::hstring RobocopyViewModel::RecordFile()
	{
		throw std::logic_error{"Dead code"};
	}
	void RobocopyViewModel::RecordFile(winrt::hstring value)
	{
		if (value.empty())
			return;

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

	static auto backSlashToForwardSlash(std::wstring_view path)
	{
		std::wstring transformed;
		std::transform(path.begin(), path.end(), std::back_inserter(transformed), [](wchar_t c)
		{
			return c == L'\\' ? L'/' : c;
		});
		return transformed;
	}
	winrt::Windows::Foundation::Uri RobocopyViewModel::DestinationUri()
	{
		return winrt::Windows::Foundation::Uri{ 
			std::format(L"file://{}", backSlashToForwardSlash(m_destination))
		};
	}
	winrt::hstring RobocopyViewModel::OperationString()
	{
		if (!m_recordFile)
			return L" ";
		switch (m_recordFile->GetOperation())
		{
			case CopyOperation::Copy: return L"Copying ";
			case CopyOperation::Move: return L"Moving ";
			case CopyOperation::Delete: return L"Deleting ";
			default:
				throw std::runtime_error{ "Invalid operation" };
		}
	}
	int RobocopyViewModel::ItemCount()
	{
		return m_recordFile.has_value() ? m_recordFile->GetNumFiles() : 0;
	}
	int RobocopyViewModel::FinishedItemCount()
	{
		return m_finishedFiles;
	}
	winrt::hstring RobocopyViewModel::Source()
	{
		if (m_recordFile && m_iter && *m_iter >= m_recordFile->begin() && *m_iter < m_recordFile->end())
			return std::filesystem::path{ **m_iter }.filename().wstring().data();
		return L"---";
	}
	winrt::hstring RobocopyViewModel::Destination()
	{
		return std::filesystem::path{ std::wstring_view{m_destination }.substr(0, m_destination.size() - 1) }.filename().wstring().data();
	}
	void RobocopyViewModel::Destination(winrt::hstring value)
	{
		m_destination = value;
		raisePropertyChange(L"Destination");
	}
	double RobocopyViewModel::Percent()
	{
		if (!m_recordFile.has_value())
			return 0;

		auto const numFiles = m_recordFile->GetNumFiles();
		if (numFiles == 0)
			return 0;

		auto const result = min(100.0, static_cast<float>(m_finishedFiles) / numFiles * 100.0);
		Taskbar::SetProgressState(Global::MainHwnd, Taskbar::ProgressState::Normal);
		Taskbar::SetProgressValue(Global::MainHwnd, result);
		return result;
	}
	void RobocopyViewModel::Start()
	{
		if (!m_recordFile)
			return;
		ProcessIOUpdater::Start(std::chrono::milliseconds{ 300 });
		m_status = Status::Running; 
		concurrency::create_task([this]
		{
			m_countItemTask.get();

			while (*m_iter != m_recordFile->end() && m_status == Status::Running)
			{
				Global::UIThread.TryEnqueue([this] {raisePropertyChange(L"Source"); });
				if (canUseRobocopy())
				{
					m_process.emplace(getRobocopyArg());
					SetHandle(m_process->Handle());
					auto const ret = m_process->WaitForExit();
				}
				else
				{
					//use fallback
					std::wstring_view destinationView{ m_destination };
					
					Fallback::CopyAddSuffix(
						**m_iter, 
						std::wstring_view{ destinationView.substr(0, destinationView.find_last_not_of(L"/\\") + 1) },
						m_recordFile->GetOperation() == CopyOperation::Move
					);
				}
				m_finishedFiles += m_recordFile->GetNumFiles(m_recordFile->IndexOf(*m_iter));
				Global::UIThread.TryEnqueue([this] 
				{ 
					raisePropertyChange(L"Percent");
					raisePropertyChange(L"FinishedItemCount");
				});
				++*m_iter;
			}
			if (*m_iter == m_recordFile->end())
			{
				Notification::SendSuccess(std::format(L"Successfully {} {} files",
					[this] {
						switch (m_recordFile->GetOperation())
						{
							case CopyOperation::Copy: return L"copied";
							case CopyOperation::Move: return L"moved";
							case CopyOperation::Delete: return L"deleted";
						}
					}(), m_finishedFiles).data(), m_destination);
				std::filesystem::remove(m_recordFile->GetPath().data());
				m_finishEvent(*this, FinishState::Success);
			}
		});
	}
	void RobocopyViewModel::Pause()
	{
		m_status = Status::Pause;
	}
	void RobocopyViewModel::Cancel()
	{
		m_status = Status::Cancel;
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
		args.moveFilesAndDirs.value = (m_recordFile->GetOperation() == CopyOperation::Move);
		if (std::filesystem::is_directory(**m_iter))
		{
			args.sourceDir = **m_iter;
			args.destinationDir += L"\\" + std::filesystem::path{ **m_iter }.filename().wstring();
			args.copySubDirectoriesIncludeEmpty.value = true;
		}
		else
		{
			std::filesystem::path path{ **m_iter };
			args.sourceDir = path.parent_path().wstring();
			args.files.push_back(path.filename().wstring());
		}
		return args;
	}

	bool RobocopyViewModel::canUseRobocopy() const
	{
		auto const fileName = std::filesystem::path{ **m_iter }.filename().wstring();
		return !std::filesystem::exists(
			std::format(LR"({}\{})", m_destination.data(), fileName)
		);
	}
}
