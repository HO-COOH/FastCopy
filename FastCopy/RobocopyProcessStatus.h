#pragma once
#include "NewFile.h"
#include "NewDir.h"

struct RobocopyProcessStatus
{
	uint64_t m_copiedBytes;
	NewFile m_currentFile;
	NewDir m_currentDir;

	void DebugSize() const;
};
