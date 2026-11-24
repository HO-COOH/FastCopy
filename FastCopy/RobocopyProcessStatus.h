#pragma once
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <optional>
#include "NewFile.h"
#include "NewDir.h"
#include "RobocopyJobFile.h"
#include "RobocopyProcess.h"

struct RobocopyProcessStatus
{
	std::atomic_uint64_t m_copiedBytes{};
	NewFile m_currentFile;
	NewDir m_currentDir;
	std::filesystem::path m_sourceRootDir;
	std::filesystem::path m_destRootDir;
	std::optional<RobocopyJobFile> m_jobFile;
	std::optional<RobocopyProcess> m_process;
	int32_t m_itemFileCount{};
	int m_finishedFiles{};
	bool m_hasError = false;
	bool m_deleteDestRoot = false;

	void DebugSize() const;
};
