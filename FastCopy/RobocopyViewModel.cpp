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
#include "RobocopyArgs.h"
#include "Taskbar.h"
#include "Notification.h"
#include "Fallback.h"
#include "ShellCopy.h"
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include "ResourceHelper.h"

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
			case CopyOperation::Copy: return ToCapital(GetStringResource(L"CopyOperation"));
			case CopyOperation::Move: return ToCapital(GetStringResource(L"MoveOperation"));
			case CopyOperation::Delete: return ToCapital(GetStringResource(L"DeleteOperation"));
			default: throw std::runtime_error{ "Invalid operation" };
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
		std::filesystem::path path{ std::wstring_view{m_destination }};
		if (auto pathName = path.filename().wstring(); !pathName.empty())
			return pathName.data();
		else
			return path.parent_path().wstring().data();
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
		Taskbar::SetProgressValue(Global::MainHwnd, std::clamp<int>(result, 1, 100));
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
				if (canUseShellCopy())
				{
					ShellCopy::Move(**m_iter, m_destination);
					m_finishedFiles += m_recordFile->GetNumFiles(m_recordFile->IndexOf(*m_iter));
				}
				else if (canUseRobocopy())
				{
					m_process.emplace(getRobocopyArg());
					SetHandle(m_process->Handle());
					auto const ret = m_process->WaitForExit();
					m_finishedFiles += m_recordFile->GetNumFiles(m_recordFile->IndexOf(*m_iter));
				}
				else
				{
					//use fallback
					std::wstring_view destinationView{ m_destination };
					Global::UIThread.TryEnqueue([this, value = **m_iter] {
						m_duplicateFiles.Append({
							value,
							std::format(LR"({}\{})", m_destination.data(), std::filesystem::path{ value }.filename().wstring())
						});
					});
					m_hasDuplicates = true;
				}
				raiseProgressChange();
				++*m_iter;
			}
			if (*m_iter == m_recordFile->end())
			{
				onNormalRobocopyFinished();
			}
			if (m_hasConfirmed)
				ConfirmDuplicates();
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
	winrt::Windows::Foundation::IReference<bool> RobocopyViewModel::UseSource()
	{
		if (m_useSourceCount == 0)
			return false;
		else if (m_useSourceCount < m_duplicateFiles.Size())
			return nullptr;
		else
			return true;
	}
	void RobocopyViewModel::UseSource(winrt::Windows::Foundation::IReference<bool> value)
	{
		if (!value) return;
		for (auto fileCompare : m_duplicateFiles)
			fileCompare.File1().Selected(value.GetBoolean());
	}
	winrt::Windows::Foundation::IReference<bool> RobocopyViewModel::UseDestination()
	{
		if (m_useDestinationCount == 0)
			return false;
		else if (m_useDestinationCount < m_duplicateFiles.Size())
			return nullptr;
		else
			return true;
	}
	void RobocopyViewModel::UseDestination(winrt::Windows::Foundation::IReference<bool> value)
	{
		if (!value) return;
		for (auto fileCompare : m_duplicateFiles)
			fileCompare.File2().Selected(value.GetBoolean());
	}

	void RobocopyViewModel::ConfirmDuplicates()
	{
		m_hasConfirmed = true;
		if (!m_duplicateFileTask.has_value())
		{
			m_duplicateFileTask.emplace();
			std::copy(m_duplicateFiles.begin(), m_duplicateFiles.end(), std::back_inserter(*m_duplicateFileTask));
			m_duplicateFiles.Clear();
			m_duplicateFileTaskIter = m_duplicateFileTask->begin();
		}

		while(m_duplicateFileTaskIter != m_duplicateFileTask->end() && m_status == Status::Running)
		{
			auto duplicate = **m_duplicateFileTaskIter;
			auto file1 = duplicate.File1();
			auto file2 = duplicate.File2();
			auto const file1Selected = file1.Selected();
			auto const file2Selected = file2.Selected();

			if (file1Selected && file2Selected)
			{
				std::wstring_view destinationView{ m_destination };
				Fallback::CopyAddSuffix(
					file1.Path().data(),
					std::wstring_view{ destinationView.substr(0, destinationView.find_last_not_of(L"/\\") + 1) },
					m_recordFile->GetOperation() == CopyOperation::Move
				);
			}
			else if (file1Selected)
			{
				try {
					std::filesystem::copy(file1.Path().data(), m_destination.data(), std::filesystem::copy_options::overwrite_existing);
				}
				catch (std::exception const& e)
				{
					OutputDebugStringA(e.what());
				}
			}
			++m_finishedFiles;
			++*m_duplicateFileTaskIter;
			raiseProgressChange();
		}
		if (*m_duplicateFileTaskIter == m_duplicateFileTask->end())
			onFallbackFinished();
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
		std::filesystem::path sourcePath{ **m_iter };
		auto const fileName = sourcePath.filename().wstring();
		return !std::filesystem::exists(
			std::format(LR"({}\{})", m_destination.data(), fileName)
		) || sourcePath.parent_path() == std::filesystem::path{ m_destination.data() };
	}
	bool RobocopyViewModel::canUseShellCopy() const
	{
		return m_recordFile->GetOperation() == CopyOperation::Move &&
			ShellCopy::IsInSameDiskPartition(**m_iter, m_destination);
	}
	void RobocopyViewModel::onNormalRobocopyFinished()
	{
		if (m_hasDuplicates)
		{
			m_canContinue = true;
			Taskbar::SetProgressState(Global::MainHwnd, Taskbar::ProgressState::Paused);
			Global::UIThread.TryEnqueue([this] {raisePropertyChange(L"CanContinue"); });
		}
		else
			onAllFinished();
	}
	void RobocopyViewModel::onFallbackFinished()
	{
		onAllFinished();
	}
	void RobocopyViewModel::onAllFinished()
	{
		auto formatString = GetStringResource(L"CopyFinishNotificationFormatString");
		Notification::SendSuccess(
			std::vformat(std::wstring_view{formatString}, std::make_wformat_args(OperationString().data(), m_finishedFiles)).data(), 
			m_destination
		);
		std::filesystem::remove(m_recordFile->GetPath().data());
		m_finishEvent(*this, FinishState::Success);
	}

	winrt::hstring RobocopyViewModel::finishedOperationString()
	{
		switch (m_recordFile->GetOperation())
		{
			case CopyOperation::Copy: return GetStringResource(L"Copied");
			case CopyOperation::Move: return GetStringResource(L"Moved");
			case CopyOperation::Delete: return GetStringResource(L"Deleted");
		}
	}

	void RobocopyViewModel::raiseProgressChange()
	{
		Global::UIThread.TryEnqueue([this] 
		{ 
			raisePropertyChange(L"Percent");
			raisePropertyChange(L"FinishedItemCount");
		});
	}
}
