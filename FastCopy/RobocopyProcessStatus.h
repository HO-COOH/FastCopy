#pragma once
#include "NewFile.h"
#include "NewDir.h"

struct RobocopyProcessStatus
{
	uint64_t m_copiedBytes;
	NewFile m_currentFile;
	NewDir m_currentDir;

#if (defined DEBUG) || (defined _DEBUG)
	void DebugSize() const;
#endif
};
