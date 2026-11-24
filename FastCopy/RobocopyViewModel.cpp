#include "pch.h"
#include "RobocopyViewModel.h"
#if __has_include("RobocopyViewModel.g.cpp")
#include "RobocopyViewModel.g.cpp"
#endif
#include <filesystem>
#include <unordered_map>
#include "ReadableUnitConverter.h"
#include "Global.h"
#include "RobocopyArgs.h"
#include "RobocopyProcess.h"
#include "RobocopyProcessStatus.h"
#include "Taskbar.h"
#include "Notification.h"
#include "Fallback.h"
#include "ShellCopy.h"
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include "ResourceHelper.h"
#include "FileCompareViewModel.h"
#include "FileInfoViewModel.h"
#include "RenameUtils.h"
#include "Settings.h"
#include "EmptyFolder.h"
#include "PathUtils.h"

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

static int clampRobocopyMT()
{
	static auto value = std::clamp<int>(std::thread::hardware_concurrency(), RobocopyArgsBuilder::MT_Min, RobocopyArgsBuilder::MT_Max);
	return value;
}

template<typename Char>
static constexpr std::basic_string_view<Char> extractFilename(std::basic_string_view<Char> path) noexcept
{
	constexpr Char separators[]{ Char{ '\\' }, Char{ '/' }, Char{ '\0' } };
	while (!path.empty() && (path.back() == Char{ '\\' } || path.back() == Char{ '/' }))
		path.remove_suffix(1);
	auto const pos = path.find_last_of(separators);
	return pos == std::basic_string_view<Char>::npos ? path : path.substr(pos + 1);
}

namespace winrt::FastCopy::implementation
{
	RobocopyViewModel::RobocopyViewModel(winrt::hstring const& recordFile, winrt::hstring const& destination) : 
		m_recordFile{recordFile},
		m_destination{destination},
		m_countItemTask{ countItemsAsync() }
	{
	}

	RobocopyViewModel::~RobocopyViewModel() = default;

	wil::task<TaskFile::TaskCount> RobocopyViewModel::countItemsAsync()
	{
		co_await winrt::resume_background();
		co_return m_recordFile.ComputeCountAndSize();
	}

	winrt::hstring RobocopyViewModel::OperationString()
	{
		switch (m_recordFile.GetOperation())
		{
			case CopyOperation::Copy: return ToCapital(GetStringResource(L"CopyOperation"));
			case CopyOperation::Move: return ToCapital(GetStringResource(L"MoveOperation"));
			case CopyOperation::Delete: return ToCapital(GetStringResource(L"DeleteOperation"));
			default: throw std::runtime_error{ "Invalid operation" };
		}
	}
	int RobocopyViewModel::ItemCount()
	{
		return m_taskCount.totalFiles;
	}
	int RobocopyViewModel::FinishedItemCount()
	{
		return m_finishedFiles.load(std::memory_order_relaxed);
	}
	winrt::hstring RobocopyViewModel::Source()
	{
		std::lock_guard lock{ m_sourceMutex };
		return m_source;
	}

	winrt::hstring RobocopyViewModel::Destination()
	{
		std::filesystem::path path{ std::wstring_view{m_destination }};
		if (auto pathName = path.filename().wstring(); !pathName.empty())
			return pathName.data();
		else
			return path.parent_path().wstring().data();
	}

	double RobocopyViewModel::Percent()
	{
		double percent{};
		if (m_taskCount.totalBytes)
		{
			percent = static_cast<double>(
				m_copiedBytes.load(std::memory_order_relaxed) + m_processPool.GetInFlightCopiedBytes()
			) / m_taskCount.totalBytes;
		}
		else if (auto const count = ItemCount())
		{
			//Sometimes all files are 0 bytes, we use file count as percent
			percent = static_cast<double>(m_finishedFiles.load(std::memory_order_relaxed)) / count;
		}

		return std::clamp(percent, 0.0, 1.0);
	}

	RobocopyViewModel::RobocopyWorkItem RobocopyViewModel::makeDirectoryWorkItem(std::filesystem::path const& sourcePath, int32_t itemFileCount)
	{
		RobocopyWorkItem item;
		item.displaySource = sourcePath;
		item.itemFileCount = itemFileCount;

		auto const operation = m_recordFile.GetOperation();
		if (operation == CopyOperation::Delete)
		{
			std::filesystem::path targetDir = sourcePath;
			targetDir.make_preferred();

			item.sourceRootDir = EmptyFolder::GetOrCreate();
			item.destRootDir = targetDir;
			item.deleteDestRoot = true;

			item.args.Source(item.sourceRootDir.wstring())
				.Destination(targetDir.wstring())
				.NJS()
				.NJH()
				.NP()
				.BYTES()
				.Unicode()
				.NOCOPY()
				.NODCOPY()
				.MIR()
				.R(0)
				.W(0)
				.MT(clampRobocopyMT());
		}
		else
		{
			auto const isMove = (operation == CopyOperation::Move);
			std::filesystem::path finalSourcePath = sourcePath;
			finalSourcePath.make_preferred();

			std::filesystem::path finalDestinationPath = std::filesystem::path{ m_destination.data() } / finalSourcePath.filename();
			finalDestinationPath.make_preferred();

			static std::wstring const suffix{ Settings{}.Get<winrt::hstring>(Settings::RenameSuffix, L" - Copy") };
			Utils::AddDestinationSuffixIfNeededForDirectory(finalSourcePath, finalDestinationPath, suffix);

			item.sourceRootDir = finalSourcePath;
			item.destRootDir = finalDestinationPath;

			item.args.Source(finalSourcePath.wstring())
				.Destination(finalDestinationPath.wstring())
				.V()
				.NJS()
				.NJH()
				.BYTES()
				.Unicode()
				.XC()
				.XN()
				.XO()
				.R(0)
				.W(0)
				.MT(clampRobocopyMT())
				.E();

			if (isMove)
				item.args.MOVE();
		}

		return item;
	}

	RobocopyViewModel::RobocopyWorkItem RobocopyViewModel::makeFileBatchWorkItem(
		std::filesystem::path const& parentDir,
		std::vector<std::wstring>&& fileNames,
		bool useJobFile)
	{
		RobocopyWorkItem item;
		item.displaySource = parentDir / fileNames.front();
		item.itemFileCount = static_cast<int32_t>(fileNames.size());

		auto const operation = m_recordFile.GetOperation();
		if (operation == CopyOperation::Delete)
		{
			item.sourceRootDir = EmptyFolder::GetOrCreate();
			item.destRootDir = parentDir;

			item.args.Source(item.sourceRootDir.wstring())
				.Destination(parentDir.wstring())
				.NJS()
				.NJH()
				.NP()
				.BYTES()
				.Unicode()
				.NOCOPY()
				.NODCOPY()
				.PURGE()
				.R(0)
				.W(0)
				.MT(clampRobocopyMT());
		}
		else
		{
			auto const isMove = (operation == CopyOperation::Move);
			std::filesystem::path finalDestinationPath{ m_destination.data() };
			finalDestinationPath.make_preferred();

			item.sourceRootDir = parentDir;
			item.destRootDir = finalDestinationPath;

			item.args.Source(parentDir.wstring())
				.Destination(finalDestinationPath.wstring())
				.V()
				.NJS()
				.NJH()
				.BYTES()
				.Unicode()
				.XC()
				.XN()
				.XO()
				.R(0)
				.W(0)
				.MT(clampRobocopyMT())
				//robocopy for top-level files defaults to /DCOPY:DA which copies the source folder
				//attribute to the destination folder (e.g. copying from a drive root hides the destination)
				.DCOPY(L"DT");

			// Use /MOV (files only) rather than /MOVE so moving all files out of parentDir never deletes parentDir itself
			if (isMove)
				item.args.MOV();
		}

		if (useJobFile)
		{
			item.jobFiles = std::move(fileNames);
		}
		else
		{
			std::array<std::wstring_view, 1> fileArg{ fileNames.front() };
			item.args.File(fileArg);
		}

		return item;
	}

	std::vector<RobocopyViewModel::RobocopyWorkItem> RobocopyViewModel::buildWorkItems()
	{
		struct FileBatch
		{
			std::filesystem::path parentDir;
			std::vector<std::wstring> fileNames;
		};

		auto const operation = m_recordFile.GetOperation();
		std::filesystem::path destinationBase{ m_destination.data() };
		if (!m_destination.empty())
			destinationBase.make_preferred();

		bool destOriginallyEmpty = false;
		if (operation == CopyOperation::Copy || operation == CopyOperation::Move)
		{
			std::error_code ec;
			destOriginallyEmpty = std::filesystem::is_empty(destinationBase, ec) && !ec;
		}

		std::vector<RobocopyWorkItem> workItems;
		std::vector<FileBatch> fileBatches;
		std::unordered_map<std::wstring, size_t> parentDirToBatchIndex;
		std::vector<std::pair<std::wstring, std::wstring>> pendingDuplicates;

		auto const itemCount = m_recordFile.GetItemCount();
		for (size_t i = 0; i < itemCount; ++i)
		{
			if (!m_processPool.WaitIfPaused())
				return {};

			auto const& itemPathStr = m_recordFile.GetItemPath(i);
			auto const isDirectory = m_recordFile.IsDirectory(i);
			auto const itemFileCount = m_recordFile.GetNumFiles(i);

			if (operation == CopyOperation::Move && ShellCopy::IsInSameDiskPartition(itemPathStr, m_destination))
			{
				updateCurrentSource(itemPathStr);
				ShellCopy::Move(itemPathStr, m_destination);
				destOriginallyEmpty = false;
				m_finishedFiles.fetch_add(itemFileCount, std::memory_order_relaxed);
				m_copiedBytes.fetch_add(m_recordFile.GetItemSize(i), std::memory_order_relaxed);
				raiseProgressChange();
				continue;
			}

			if (isDirectory)
			{
				workItems.push_back(makeDirectoryWorkItem(std::filesystem::path{ itemPathStr }, itemFileCount));
				continue;
			}

			std::filesystem::path sourcePath{ itemPathStr };
			auto fileName = sourcePath.filename().wstring();

			if (operation == CopyOperation::Copy || operation == CopyOperation::Move)
			{
				std::error_code existsEc;
				if (!destOriginallyEmpty && std::filesystem::exists(destinationBase / fileName, existsEc) && !existsEc)
				{
					pendingDuplicates.emplace_back(
						itemPathStr,
						(destinationBase / fileName).wstring()
					);
					m_hasDuplicates.store(true, std::memory_order_seq_cst);
					continue;
				}
			}

			std::filesystem::path parentDir = sourcePath.parent_path();
			parentDir.make_preferred();

			if (!RobocopyJobFile::CanUseInJobFile(fileName))
			{
				std::vector<std::wstring> singleFile;
				singleFile.push_back(std::move(fileName));
				workItems.push_back(makeFileBatchWorkItem(parentDir, std::move(singleFile), false));
				continue;
			}

			std::wstring batchKey = parentDir.wstring();
			if (!batchKey.empty())
				CharUpperBuffW(batchKey.data(), static_cast<DWORD>(batchKey.size()));

			auto [it, inserted] = parentDirToBatchIndex.try_emplace(std::move(batchKey), fileBatches.size());
			if (inserted)
			{
				fileBatches.push_back(FileBatch{ .parentDir = std::move(parentDir), .fileNames = {} });
			}
			fileBatches[it->second].fileNames.push_back(std::move(fileName));
		}

		if (!pendingDuplicates.empty() && m_processPool.GetStatus() == ProcessStatus::Running)
		{
			Global::UIThread.TryEnqueue([this, duplicates = std::move(pendingDuplicates)]() mutable {
				for (auto& [src, dst] : duplicates)
				{
					m_duplicateFiles.Append(winrt::make<FileCompareViewModel>(src, dst, *this));
				}
			});
		}

		for (auto& batch : fileBatches)
		{
			if (m_processPool.GetStatus() != ProcessStatus::Running)
				return {};

			bool const useJobFile = batch.fileNames.size() > 1;
			workItems.push_back(makeFileBatchWorkItem(batch.parentDir, std::move(batch.fileNames), useJobFile));
		}

		return workItems;
	}

	void RobocopyViewModel::launchWorkItem(size_t currentIndex, RobocopyWorkItem&& item)
	{
		updateCurrentSource(item.displaySource.native());
		raiseProgressChange();

		auto& status = m_processPool[currentIndex];
		status.m_sourceRootDir = std::move(item.sourceRootDir);
		status.m_destRootDir = std::move(item.destRootDir);
		status.m_itemFileCount = item.itemFileCount;
		status.m_deleteDestRoot = item.deleteDestRoot;

		auto failLaunch = [this, currentIndex](std::string_view message)
		{
			auto& status = m_processPool[currentIndex];
			status.m_hasError = true;
			status.m_jobFile.reset();
			abortWithError(message);
			(void)m_processPool.OnProcessFinished();
		};

		auto args = std::move(item.args);
		if (!item.jobFiles.empty())
		{
			try
			{
				auto& jobFile = status.m_jobFile.emplace(item.jobFiles);
				args.JOB(jobFile.PathView());
			}
			catch (winrt::hresult_error const& e)
			{
				failLaunch(std::format("Failed to create robocopy job file (0x{:08X}): {}", static_cast<uint32_t>(e.code()), winrt::to_string(e.message())));
				return;
			}
		}

		try
		{
			m_processPool.Launch(
				currentIndex,
				args,
				overloaded
				{
					/*onInjectionFailed*/
					[this, currentIndex](RobocopyProcess::InjectionFailed&& failure)
					{
						m_processPool[currentIndex].m_hasError = true;
						abortWithError(failure.message);
					},
					/*onError*/
					[this, currentIndex](Error&& error)
					{
						if (m_processPool.GetStatus() == ProcessStatus::Failed)
							return;

						//A robocopy ERROR (e.g. access denied). Mark this process failed, collect the message
						//and turn the progress bar red. All errors are reported when the operation finishes.
						m_processPool[currentIndex].m_hasError = true;
						appendError(error.error);
						Global::UIThread.TryEnqueue([this] {
							if (m_state != TaskbarState::Failed)
							{
								m_state = TaskbarState::Error;
								raisePropertyChange(L"State");
							}
						});
					},
					/*onProgress*/
					[this, currentIndex](float progress)
					{
						auto& status = m_processPool[currentIndex];
						status.m_copiedBytes.store(
							static_cast<uint64_t>(status.m_currentFile.bytes * progress / 100.f),
							std::memory_order_relaxed
						);
						raiseProgressChange();
					},
					/*onNewFile*/
					[this, currentIndex](NewFile&& newFile)
					{
						auto& status = m_processPool[currentIndex];
						if (status.m_currentFile == newFile)
							return;

						auto const hasPreviousFile = static_cast<bool>(status.m_currentFile);
						auto const previousFileSize = status.m_copiedBytes.exchange(0, std::memory_order_relaxed);

						updateCurrentSource(newFile.name);
						status.m_currentFile = std::move(newFile);

#if (defined DEBUG) || (defined _DEBUG)
						status.DebugSize();
#endif

						if (hasPreviousFile)
						{
							++status.m_finishedFiles;
							m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
							m_copiedBytes.fetch_add(previousFileSize, std::memory_order_relaxed);
						}
						raiseProgressChange();
					},
					/*onNewFolder*/
					[this, currentIndex](NewDir&& newDir)
					{
						m_processPool[currentIndex].m_currentDir = std::move(newDir);
					},
					/*on same*/
					[this, currentIndex](Same&& sameFile)
					{
						auto& status = m_processPool[currentIndex];
						++status.m_finishedFiles;
						m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
						m_copiedBytes.fetch_add(sameFile.bytes, std::memory_order_relaxed);
						status.m_copiedBytes.store(0, std::memory_order_relaxed);
						raiseProgressChange();
					},
					/*on different*/
					[this, currentIndex](Conflict&& conflict)
					{
						//Robocopy skipped it, its bytes are counted by ConfirmDuplicates() once it is actually copied
						auto const& status = m_processPool[currentIndex];
						auto conflictPath = Utf8ToPath(conflict.name);
						std::filesystem::path srcConflictPath;
						std::filesystem::path dstConflictPath;
						if (conflictPath.is_absolute())
						{
							srcConflictPath = conflictPath;
							std::error_code relEc;
							auto rel = std::filesystem::relative(conflictPath, status.m_sourceRootDir, relEc);
							dstConflictPath = (!relEc && !rel.empty()) ? (status.m_destRootDir / rel) : (status.m_destRootDir / conflictPath.filename());
						}
						else
						{
							srcConflictPath = status.m_sourceRootDir / conflictPath;
							auto const& currentDestDir = status.m_currentDir.fullPath;
							dstConflictPath = (currentDestDir.empty() ? status.m_destRootDir : Utf8ToPath(currentDestDir)) / conflictPath;
						}

						Global::UIThread.TryEnqueue([this, srcStr = srcConflictPath.wstring(), dstStr = dstConflictPath.wstring()]() mutable {
							m_duplicateFiles.Append(winrt::make<FileCompareViewModel>(srcStr, dstStr, *this));
						});
						m_hasDuplicates.store(true, std::memory_order_seq_cst);
					},
					/*onExistingDir*/
					[this, currentIndex](ExistingDir&& existingDir)
					{
						auto& status = m_processPool[currentIndex];
						status.m_currentDir.count = existingDir.count;

						std::filesystem::path currentSubfolderPath = Utf8ToPath(existingDir.path);
						std::error_code relEc;
						std::filesystem::path relativePath = std::filesystem::relative(currentSubfolderPath, status.m_sourceRootDir, relEc);
						std::filesystem::path destinationSubfolderPath = (!relEc && !relativePath.empty())
							? (status.m_destRootDir / relativePath).lexically_normal()
							: status.m_destRootDir;

						status.m_currentDir.fullPath = winrt::to_string(destinationSubfolderPath.wstring());
					},
					/*onProcessExit*/
					[this, currentIndex](RobocopyProcess::Exit)
					{
						auto& status = m_processPool[currentIndex];
						status.m_jobFile.reset();

						if (status.m_currentFile)
						{
							++status.m_finishedFiles;
							m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
						}
						m_copiedBytes.fetch_add(status.m_copiedBytes.exchange(0, std::memory_order_relaxed), std::memory_order_relaxed);

						// After robocopy /MIR finishes, the folder is empty but still exists - delete it
						if (status.m_deleteDestRoot &&
							!status.m_hasError &&
							m_processPool.GetStatus() == ProcessStatus::Running)
						{
							std::error_code ec;
							std::filesystem::remove_all(status.m_destRootDir, ec);
							++status.m_finishedFiles;
							m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
						}

						if (status.m_hasError && status.m_finishedFiles < status.m_itemFileCount)
						{
							auto const remaining = status.m_itemFileCount - status.m_finishedFiles;
							status.m_finishedFiles += remaining;
							m_finishedFiles.fetch_add(remaining, std::memory_order_relaxed);
						}

						if (m_processPool.OnProcessFinished())
						{
							completeRobocopyStage();
						}
					},
					[this, currentIndex](ExtraDir&&)
					{
						//A sub-folder removed by robocopy /MIR, include it in finished files
						++m_processPool[currentIndex].m_finishedFiles;
						m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
						raiseProgressChange();
					},
					[this, currentIndex](ExtraFile&& deletedFile)
					{
						updateCurrentSource(deletedFile.name);
						++m_processPool[currentIndex].m_finishedFiles;
						m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
						m_copiedBytes.fetch_add(deletedFile.bytes, std::memory_order_relaxed);
						raiseProgressChange();
					}
				}
			);
		}
		catch (RobocopyProcess::InjectionFailed& e)
		{
			failLaunch(e.message);
		}
		catch (std::exception const& e)
		{
			failLaunch(std::format("Failed to start robocopy.exe: {}", e.what()));
		}
	}

	void RobocopyViewModel::appendError(std::string_view error)
	{
		auto const wideError = winrt::to_hstring(error);
		std::lock_guard lock{ m_errorsMutex };
		if (m_errorText.empty())
			m_errorText = L"The operation finished with the following errors:\n\n";
		m_errorText += wideError;
		m_errorText += L'\n';
	}

	void RobocopyViewModel::abortWithError(std::string_view message)
	{
		appendError(message);
		m_processPool.Fail();

		Global::UIThread.TryEnqueue([this] {
			m_duplicateFiles.Clear();
		});

		onAllFinished();
	}

	void RobocopyViewModel::completeRobocopyStage()
	{
		auto const st = m_processPool.GetStatus();
		if (st == ProcessStatus::Cancel || st == ProcessStatus::Failed)
			return;

		m_robocopyStageFinished.store(true, std::memory_order_seq_cst);
		if (!m_hasDuplicates.load(std::memory_order_seq_cst) || m_duplicatesFinished.load(std::memory_order_seq_cst))
		{
			onAllFinished();
			return;
		}

		onNormalRobocopyFinished();
	}

	winrt::fire_and_forget RobocopyViewModel::runCopyLoopAsync()
	{
		//Wait for counting+sizing to finish before iterating: this guarantees ItemCount() is the
		//real total by the time any file is marked finished.
		auto const taskCount = co_await std::move(m_countItemTask);
		if (!Global::UIThread.m_queue || Global::UIThread.m_queue.HasThreadAccess())
		{
			m_taskCount = taskCount;
			raisePropertyChange(L"ItemCount");
			co_await winrt::resume_background();
		}
		else
		{
			Global::UIThread.TryEnqueue([this, taskCount] {
				m_taskCount = taskCount;
				raisePropertyChange(L"ItemCount");
			});
		}

		auto workItems = buildWorkItems();
		if (m_processPool.GetStatus() != ProcessStatus::Running)
			co_return;

		if (workItems.empty())
		{
			completeRobocopyStage();
			co_return;
		}

		m_processPool.Preallocate(workItems.size());

		for (size_t i = 0; i < workItems.size(); ++i)
		{
			if (!m_processPool.AcquireSlot())
			{
				m_processPool.CancelUnlaunched(workItems.size() - i);
				co_return;
			}

			launchWorkItem(i, std::move(workItems[i]));
		}
	}

	void RobocopyViewModel::Start()
	{
		ProcessIOUpdater::Start(std::chrono::milliseconds{ 100 }, Global::UIThread.m_queue);
		m_processPool.SetStatus(ProcessStatus::Running);
		//Keep the machine awake for the whole operation. If the PC idle-sleeps mid-copy, on wake the
		//overlapped pipe read from robocopy can be aborted, which stalls robocopy (its stdout stops
		//being drained) while the UI stays responsive. ES_SYSTEM_REQUIRED only suppresses the idle
		//timeout - a deliberate sleep by the user still works.
		SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
		runCopyLoopAsync();
		if (m_processPool.GetStatus() == ProcessStatus::Running)
			m_state = TaskbarState::Normal;
		raisePropertyChange(L"State");
	}
	void RobocopyViewModel::TogglePause()
	{
		auto const currentStatus = m_processPool.GetStatus();
		if (currentStatus == ProcessStatus::Cancel || currentStatus == ProcessStatus::Failed)
			return;

		if (currentStatus == ProcessStatus::Running)
		{
			m_processPool.Pause();
			m_state = TaskbarState::Paused;
		}
		else
		{
			m_processPool.Resume();
			m_state = TaskbarState::Normal;
		}
		raisePropertyChange(L"State");
	}
	void RobocopyViewModel::Cancel()
	{
		if (m_processPool.Cancel())
			SetThreadExecutionState(ES_CONTINUOUS); //cancelled - stop keeping the machine awake
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
		if (!m_duplicateFileTask.has_value())
		{
			m_duplicateFileTask.emplace();
			std::copy(m_duplicateFiles.begin(), m_duplicateFiles.end(), std::back_inserter(*m_duplicateFileTask));
			m_duplicateFiles.Clear();
			m_duplicateFileTaskIter = m_duplicateFileTask->begin();
		}

		while(*m_duplicateFileTaskIter != m_duplicateFileTask->end() && m_processPool.GetStatus() == ProcessStatus::Running)
		{
			auto duplicate = **m_duplicateFileTaskIter;
			auto file1 = winrt::get_self<FileInfoViewModel>(duplicate.File1());
			auto file2 = winrt::get_self<FileInfoViewModel>(duplicate.File2());
			auto const file1Selected = file1->Selected();
			auto const file2Selected = file2->Selected();

			//Added to m_copiedBytes only after this item is done below, but read now, as a move removes the source
			//m_totalSize includes the source whatever is chosen, so keeping the destination counts as done too
			std::error_code ec;
			auto const sourceSize = std::filesystem::file_size(file1->Path().data(), ec);

			if (file1Selected && file2Selected)
			{
				static std::wstring const suffix{ Settings{}.Get<winrt::hstring>(Settings::RenameSuffix, L" - Copy") };
				//rename
				Fallback::CopyAddSuffix(
					file1->Path().data(),
					std::filesystem::path{ file2->Path().data() },
					suffix,
					m_recordFile.GetOperation() == CopyOperation::Move
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
			m_finishedFiles.fetch_add(1, std::memory_order_relaxed);
			if (!ec)
				m_copiedBytes.fetch_add(sourceSize, std::memory_order_relaxed);
			++*m_duplicateFileTaskIter;
			raiseProgressChange();
		}
		if (*m_duplicateFileTaskIter == m_duplicateFileTask->end())
		{
			m_duplicatesFinished.store(true, std::memory_order_seq_cst);
			if (m_robocopyStageFinished.load(std::memory_order_seq_cst))
				onAllFinished();
		}
	}

	void RobocopyViewModel::onNormalRobocopyFinished()
	{
		if (m_hasDuplicates.load(std::memory_order_acquire))
		{
			Global::UIThread.TryEnqueue([this] {
				m_state = TaskbarState::Paused;
				raisePropertyChange(L"State");
				raisePropertyChange(L"Percent");
				raisePropertyChange(L"FinishedItemCount");
			});
		}
		else
		{
			raiseProgressChange();
			onAllFinished();
		}
	}

	void RobocopyViewModel::onAllFinished()
	{
		if (m_allFinished.exchange(true))
			return; //already finished

		//Clear the keep-awake requirement so the machine can idle-sleep again. It was set on the UI
		//thread in Start(), and an ES_CONTINUOUS requirement is tied to the setting thread, so clear it
		//on the UI thread too (onAllFinished often runs on the io-callback thread).
		Global::UIThread.TryEnqueue([] { SetThreadExecutionState(ES_CONTINUOUS); });

		bool hasErrors{};
		{
			std::lock_guard lock{ m_errorsMutex };
			hasErrors = !m_errorText.empty();
		}

		if (hasErrors)
		{
			//The operation finished but some items failed. Publish the joined error text (shown by CopyDialogWindow's ErrorText) and mark the operation failed.
			m_processPool.SetStatus(ProcessStatus::Failed);

			Global::UIThread.TryEnqueue([this] {
				m_state = TaskbarState::Failed;
				raisePropertyChange(L"ErrorText");
				raisePropertyChange(L"State");
			});

			Stop();
			auto formatString = GetStringResource(L"CopyFailedNotificationFormatString");
			Notification::SendFailed(
				std::vformat(std::wstring_view{ formatString }, std::make_wformat_args(m_destination)).data(),
				m_destination
			);
			return; //keep the window open (do not auto-exit) so the user can see the red progress bar and the errors
		}

		auto formatString = GetStringResource(L"CopyFinishNotificationFormatString");
		auto operationString = OperationString();
		auto operationStringData = operationString.data();
		auto const finishedFilesCount = m_finishedFiles.load(std::memory_order_relaxed);
		Notification::SendSuccess(
			std::vformat(std::wstring_view{formatString}, std::make_wformat_args(operationStringData, finishedFilesCount, m_destination)).data(),
			m_destination
		);
		Stop();

		//Force one final progress update: raiseProgressChange() is throttled, so the last per-file
		//notification may have been dropped and the bar would stay short of 100%. Enqueued before the
		//exit below, so it is dispatched first.
		Global::UIThread.TryEnqueue([this] {
			raisePropertyChange(L"Source");
			raisePropertyChange(L"Percent");
			raisePropertyChange(L"FinishedItemCount");
		});

		if (!Settings{}.Get(Settings::DevMode, false))
		{
			Global::UIThread.TryEnqueue([] {
				winrt::Microsoft::UI::Xaml::Application::Current().Exit();
			});
		}
	}

	void RobocopyViewModel::updateCurrentSource(std::wstring_view path)
	{
		winrt::hstring source{ extractFilename(path) };
		std::lock_guard lock{ m_sourceMutex };
		m_source = std::move(source);
	}

	void RobocopyViewModel::updateCurrentSource(std::string_view utf8Path)
	{
		auto source = winrt::to_hstring(extractFilename(utf8Path));
		std::lock_guard lock{ m_sourceMutex };
		m_source = std::move(source);
	}

	void RobocopyViewModel::raiseProgressChange()
	{
		constexpr auto k_ProgressUpdateInterval = std::chrono::milliseconds{ 100 };
		auto const now = std::chrono::steady_clock::now();
		if (now - m_lastProgressUpdate.load(std::memory_order_relaxed) < k_ProgressUpdateInterval)
			return;
		m_lastProgressUpdate.store(now, std::memory_order_relaxed);

		Global::UIThread.TryEnqueue([this] 
		{ 
			raisePropertyChange(L"Source");
			raisePropertyChange(L"Percent");
			raisePropertyChange(L"FinishedItemCount");
		});
	}

	FastCopy::TaskbarState RobocopyViewModel::State()
	{
		return m_state;
	}

	winrt::hstring RobocopyViewModel::ErrorText()
	{
		std::lock_guard lock{ m_errorsMutex };
		return winrt::hstring{ m_errorText };
	}
}
