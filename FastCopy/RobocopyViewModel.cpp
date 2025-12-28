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
#include <pplawait.h>
#include <numeric>
#include "FileInfoViewModel.h"
#include "Ntdll.h"
#include "ViewModelLocator.h"
#include "RenameUtils.h"
#include "Settings.h"
#include "EmptyFolder.h"

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

static int clampRobocopyMT()
{
	static auto value = std::clamp<int>(std::thread::hardware_concurrency(), RobocopyArgsBuilder::MT_Min, RobocopyArgsBuilder::MT_Max);
	return value;
}

namespace winrt::FastCopy::implementation
{
	void RobocopyViewModel::RecordFile(winrt::hstring value)
	{
		if (value.empty())
			return;

		m_recordFile.emplace(value);
		m_recordFile->GetNumFiles();
		m_iter = m_recordFile->begin();
		m_recordFileBegin = m_recordFile->begin();
		m_recordFileEnd = m_recordFile->end();

		m_countItemTask = concurrency::create_task([this]
		{
			m_totalSize = m_recordFile->GetTotalSize();
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
			std::format(L"file:///{}", backSlashToForwardSlash(m_destination))
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
		return m_recordFile.has_value() && m_countItemTask.is_done() ? m_recordFile->GetNumFiles() : 0;
	}
	int RobocopyViewModel::FinishedItemCount()
	{
		return m_finishedFiles;
	}
	winrt::hstring RobocopyViewModel::Source()
	{
		return winrt::hstring{ m_sourceForUI ? m_sourceForUI->filename().wstring() : L"---" };
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
		if (!m_recordFile.has_value() || !m_countItemTask.is_done())
			return 0;

		double percent{};
		if (m_totalSize)
		{
			std::lock_guard lock{ m_processStatusMutex };
			percent = (
				static_cast<double>(m_copiedBytes) + 
				std::accumulate(m_perProcessStatus.begin(), m_perProcessStatus.end(), 0ull, [](uint64_t sum, RobocopyProcessStatus const& status) {
					return sum + status.m_copiedBytes;
				})
			) / m_totalSize;
		}
		else
		{
			//Sometimes all files are 0 bytes, we use file count as percent
			percent = static_cast<double>(m_finishedFiles) / ItemCount();
		}
		return percent;
	}

	winrt::Windows::Foundation::IAsyncAction RobocopyViewModel::Start()
	{
		if (!m_recordFile)
			co_return;
		//co_await m_countItemTask;
		ProcessIOUpdater::Start(std::chrono::milliseconds{ 100 }, Global::UIThread.m_queue);
		m_status = Status::Running; 
		concurrency::create_task([this]
		{
			//m_countItemTask.get();
			while (*m_iter != m_recordFile->end() && m_status == Status::Running)
			{
				Global::UIThread.TryEnqueue([this, value = **m_iter] {
					m_sourceForUI.emplace(std::move(value));
					raisePropertyChange(L"Source"); 
				});
				if (canUseShellCopy())
				{
					auto currentItemSize = TaskFile::GetSizeOfPath(**m_iter);
					ShellCopy::Move(**m_iter, m_destination);
					m_finishedFiles += m_recordFile->GetNumFiles(m_recordFile->IndexOf(*m_iter));
					m_copiedBytes += currentItemSize;
				}
				else if (canUseRobocopy())
				{
					auto currentIndex = m_process.size();
					{
						std::lock_guard lock{ m_processStatusMutex };
						m_perProcessStatus.emplace_back();
					}

					// Capture whether this is a folder delete operation and the target path
					std::filesystem::path folderToDeleteAfterRobocopy;
					if (m_recordFile->GetOperation() == CopyOperation::Delete)
					{
						std::filesystem::path currentPath{ **m_iter };
						std::error_code ec;
						if (std::filesystem::is_directory(currentPath, ec) && !ec)
							folderToDeleteAfterRobocopy = std::move(currentPath);
					}

					m_process.emplace_back(std::make_unique<RobocopyProcess>(getRobocopyArg(),
						overloaded
						{
							/*onProgress*/
							[this, currentIndex](float progress)
							{
								m_perProcessStatus[currentIndex].m_copiedBytes = m_perProcessStatus[currentIndex].m_currentFile.bytes * progress / 100.f;
								raiseProgressChange();
							},
							/*onNewFile*/
							[this, currentIndex](NewFile&& newFile)
							{
								if (m_perProcessStatus[currentIndex].m_currentFile == newFile)
									return;

								//std::cout << "!!!New file: " << m_perProcessStatus[currentIndex].m_currentFile.name << " -> " << m_perProcessStatus[currentIndex].m_currentFile.bytes << '\n';

								auto const hasPreviousFile = static_cast<bool>(m_perProcessStatus[currentIndex].m_currentFile);
								auto const previousFileSize = std::exchange(m_perProcessStatus[currentIndex].m_copiedBytes, 0);

								m_perProcessStatus[currentIndex].m_currentFile = std::move(newFile);

#if (defined DEBUG) || (defined _DEBUG)
								m_perProcessStatus[currentIndex].DebugSize();
#endif

								if (!hasPreviousFile)
									return;

								++m_finishedFiles;
								m_copiedBytes += previousFileSize;
								if (m_finishedFiles == ItemCount())
									onNormalRobocopyFinished();
								raiseProgressChange();
							},
							/*onNewFolder*/
							[this, currentIndex](NewDir&& newDir)
							{
								m_perProcessStatus[currentIndex].m_currentDir = std::move(newDir);
							},
							/*on same*/
							[this, currentIndex](Same&& sameFile)
							{
								++m_finishedFiles;
								m_copiedBytes += sameFile.bytes;
								m_perProcessStatus[currentIndex].m_copiedBytes = 0;
								if (m_finishedFiles == ItemCount())
									onNormalRobocopyFinished();
								raiseProgressChange();
							},
							/*on different*/
							[this, currentIndex, currentSource = **m_iter](Conflict&& conflict)
							{
								auto const currentItemSize = conflict.bytes;
								Global::UIThread.TryEnqueue([this, conflict = std::move(conflict), currentDestinationFolder = m_perProcessStatus[currentIndex].m_currentDir.fullPath, currentSource = std::move(currentSource)] {
									m_duplicateFiles.Append({
										(std::filesystem::path{currentSource} / conflict.name).wstring(),
										(std::filesystem::path{currentDestinationFolder} / conflict.name).wstring()
									});
								});
								m_hasDuplicates = true;
								m_copiedBytes += currentItemSize;
							},
							/*onExistingDir*/
							[this, currentIndex, currentSource = **m_iter](ExistingDir&& existingDir)
							{
								m_perProcessStatus[currentIndex].m_currentDir.count = existingDir.count;

								// Calculate the correct destination subfolder path using recorded source path
								std::filesystem::path currentSubfolderPath{ existingDir.path };
								std::filesystem::path destinationBasePath{ m_destination.data() };

								// Get the relative path from recorded source to current subfolder
								std::filesystem::path relativePath = std::filesystem::relative(currentSubfolderPath, std::filesystem::path{ currentSource }.parent_path());

								// Calculate destination subfolder path: destination/relative_path
								std::filesystem::path destinationSubfolderPath = destinationBasePath / relativePath;

								m_perProcessStatus[currentIndex].m_currentDir.fullPath = destinationSubfolderPath.string();
							},
							/*onProcessExit*/
							[this, currentIndex, folderToDeleteAfterRobocopy = std::move(folderToDeleteAfterRobocopy)](RobocopyProcess::Exit)
							{
								if (m_perProcessStatus[currentIndex].m_currentFile)
									++m_finishedFiles;
								m_copiedBytes += std::exchange(m_perProcessStatus[currentIndex].m_copiedBytes, 0);
								
								// After robocopy /MIR finishes, the folder is empty but still exists - delete it
								if (!folderToDeleteAfterRobocopy.empty())
								{
									std::error_code ec;
									std::filesystem::remove_all(folderToDeleteAfterRobocopy, ec);
								}

								if (m_finishedFiles == ItemCount())
									onNormalRobocopyFinished();
							},
							[this](ExtraFile&& deletedFile)
							{
								++m_finishedFiles;
								m_copiedBytes += deletedFile.bytes;
								raiseProgressChange();
							}
						}
					));
					
					SetHandle(m_process.back()->Handle());
				}
				else
				{
					//use fallback
					std::wstring_view destinationView{ m_destination };
					Global::UIThread.TryEnqueue([this, value = **m_iter] {
						m_duplicateFiles.Append({
							value,
							(std::filesystem::path{m_destination.data()} / std::filesystem::path{value}.filename()).wstring()
						});
					});
					m_hasDuplicates = true;
				}

				raiseProgressChange();
				++*m_iter;
			}
			if (m_hasConfirmed)
				ConfirmDuplicates();
		});
		m_state = TaskbarState::Normal;
		co_await wil::resume_foreground(Global::UIThread.m_queue);
		raisePropertyChange(L"State");
	}
	void RobocopyViewModel::TogglePause()
	{
		if (m_status == Status::Running)
		{
			m_status = Status::Pause;
			m_state = TaskbarState::Paused;
			for (auto& process : m_process)
				NtDll::NtSuspendProcess(process->Handle());
		}
		else
		{
			m_status = Status::Running;
			m_state = TaskbarState::Normal;
			for (auto& process : m_process)
				NtDll::NtResumeProcess(process->Handle());
		}
		raisePropertyChange(L"State");
	}
	void RobocopyViewModel::Cancel()
	{
		m_status = Status::Cancel;
	}
	void RobocopyViewModel::OnUpdateCopySpeed(ProcessIoCounter::IOCounterDiff diff)
	{
		m_bytesPerSec = ReadableUnitConverter::Speed::BytesPerSec(diff.read, diff.duration);
		raisePropertyChange(L"Speed");
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
		for (auto const selected = value.GetBoolean(); auto fileCompare : m_duplicateFiles)
			fileCompare.File1().Selected(selected);
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
		for (auto const selected = value.GetBoolean(); auto fileCompare : m_duplicateFiles)
			fileCompare.File2().Selected(selected);
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
			auto file1 = winrt::get_self<FileInfoViewModel>(duplicate.File1());
			auto file2 = winrt::get_self<FileInfoViewModel>(duplicate.File2());
			auto const file1Selected = file1->Selected();
			auto const file2Selected = file2->Selected();

			if (file1Selected && file2Selected)
			{
				static std::wstring const suffix{ Settings{}.Get<winrt::hstring>(Settings::RenameSuffix, L" - Copy") };
				//rename
				;
				Fallback::CopyAddSuffix(
					file1->Path().data(),
					std::filesystem::path{ file2->Path().data() },
					suffix,
					m_recordFile->GetOperation() == CopyOperation::Move
				);
			}
			else if (file1Selected)
			{
				//overwrite
				try 
				{
					std::filesystem::copy(
						file1->Path().data(), 
						std::filesystem::path{ file2->Path().data() }.parent_path(),
						std::filesystem::copy_options::overwrite_existing
					);
				}
				catch (std::exception const& e)
				{
					MessageBox(
						NULL,
						std::format(L"Overwriting: {} -> {} failed: {}", file1->Path(), file2->Path(), winrt::to_hstring(e.what())).data(),
						L"RobocopyEx Error",
						NULL
					);
				}
			}
			++m_finishedFiles;
			++*m_duplicateFileTaskIter;
			raiseProgressChange();
		}
		if (*m_duplicateFileTaskIter == m_duplicateFileTask->end())
			onFallbackFinished();
	}

	winrt::Windows::Foundation::IAsyncOperation<uint64_t> RobocopyViewModel::GetTotalSize()
	{
		//auto result = co_await concurrency::create_task([this]()->winrt::Windows::Foundation::IAsyncA
		//if(m_countItemTask)

		co_return m_recordFile ? m_recordFile->GetTotalSize() : 0;
	}

	winrt::event_token RobocopyViewModel::Finished(winrt::Windows::Foundation::EventHandler<winrt::FastCopy::FinishState> const& handler)
	{
		return m_finishEvent.add(handler);
	}
	void RobocopyViewModel::Finished(winrt::event_token const& token) noexcept
	{
		m_finishEvent.remove(token);
	}
	RobocopyArgsBuilder RobocopyViewModel::getRobocopyArg()
	{
		RobocopyArgsBuilder args;

		switch (m_recordFile->GetOperation())
		{
			case CopyOperation::Move: [[fallthrough]];
			case CopyOperation::Copy:
			{
				std::filesystem::path sourcePath{ **m_iter };
				std::filesystem::path destinationBase{ m_destination.data() };
				auto const isFolder = std::filesystem::is_directory(sourcePath);
				auto const isMove = m_recordFile->GetOperation() == CopyOperation::Move;

				// Determine source and destination paths based on whether we're copying a file or directory
				std::filesystem::path finalSourcePath;
				std::filesystem::path finalDestinationPath;

				if (isFolder)
				{
					// For directories: source is the directory itself, destination includes the directory name
					finalSourcePath = std::move(sourcePath);
					finalDestinationPath = std::move(destinationBase) / finalSourcePath.filename();

					// Handle duplicate directory names by adding suffix if needed
					static std::wstring const suffix{ Settings{}.Get<winrt::hstring>(Settings::RenameSuffix, L" - Copy") };
					Utils::AddDestinationSuffixIfNeededForDirectory(finalSourcePath, finalDestinationPath, suffix);
				}
				else
				{
					// For files: source is the parent directory, destination is the target directory
					finalSourcePath = sourcePath.parent_path();
					finalDestinationPath = std::move(destinationBase);
				}

				// Configure common robocopy arguments
				args.Source(finalSourcePath.wstring())
					.Destination(finalDestinationPath.wstring())
					.V(true)           // Verbose output
					.NJS(true)         // No job summary
					.NJH(true)         // No job header
					.BYTES(true)       // Print sizes as bytes
					.Unicode(true)     // Output as Unicode
					.XC(true)          // Exclude changed files
					.XN(true)          // Exclude newer files
					.XO(true)          // Exclude older files
					.MT(clampRobocopyMT());

				// Configure operation-specific arguments
				if (isMove)
					args.MOVE(true);

				if (isFolder)
					args.E(true);
				else
				{
					// Specify the single file to copy
					auto fileName = sourcePath.filename().wstring();
					std::array<std::wstring_view, 1> fileArg{ fileName };
					args.File(fileArg);
				}

				break;
			}
			case CopyOperation::Delete:
			{
				std::filesystem::path targetPath{ **m_iter };
				auto const isDirectory = std::filesystem::is_directory(targetPath);

				if (isDirectory)
				{
					// For folders, use /MIR with empty source to delete the directory contents
					args.Source(EmptyFolder::GetOrCreate().c_str())
						.Destination(**m_iter)
						.NJS(true)
						.NJH(true)
						.BYTES(true)
						.Unicode(true)
						.MIR(true)
						.MT(clampRobocopyMT());
				}
				else
				{
					// For files, use /PURGE with the parent directory and specific file pattern
					auto parentPath = targetPath.parent_path();
					auto fileName = targetPath.filename().wstring();
					std::array<std::wstring_view, 1> fileArg{ fileName };

					args.Source(EmptyFolder::GetOrCreate().c_str())
						.Destination(parentPath.wstring())
						.File(fileArg)
						.NJS(true)
						.NJH(true)
						.BYTES(true)
						.Unicode(true)
						.PURGE(true)
						.MT(clampRobocopyMT());
				}
				break;
			}
		}
		

		return args;
	}

	bool RobocopyViewModel::canUseRobocopy() const
	{
		std::filesystem::path sourcePath{ **m_iter };
		std::filesystem::path destinationPath{ m_destination.data() };
		auto const fileName = sourcePath.filename().wstring();

		//If source is dirctory, robocopy can be used even with duplicated names
		if (std::filesystem::is_directory(sourcePath))
			return true;

		return
			!std::filesystem::exists(destinationPath / fileName);   //destination does not contain the same file name
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
			m_state = TaskbarState::Paused;
			raisePropertyChange(L"State");
		}
		else
		{
			raiseProgressChange();
			onAllFinished();
		}
	}
	void RobocopyViewModel::onFallbackFinished()
	{
		onAllFinished();
	}
	void RobocopyViewModel::onAllFinished()
	{
		auto formatString = GetStringResource(L"CopyFinishNotificationFormatString");
		auto operationString = OperationString();
		auto operationStringData = operationString.data();
		Notification::SendSuccess(
			std::vformat(std::wstring_view{formatString}, std::make_wformat_args(operationStringData, m_finishedFiles, m_destination)).data(),
			m_destination
		);
		std::filesystem::remove(m_recordFile->GetPath().data());
		m_finishEvent(*this, FinishState::Success);
		Stop();
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

	FastCopy::TaskbarState RobocopyViewModel::State()
	{
		return m_state;
	}
}
