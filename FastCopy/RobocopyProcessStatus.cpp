#include "pch.h"
#include "RobocopyProcessStatus.h"
#include "DebugFileSize.h"
#include <format>
#include <iostream>

void RobocopyProcessStatus::DebugSize() const
{
	std::cout <<
		std::format(
			"{}\\{}: {}\n",
			m_currentDir.fullPath,
			m_currentFile.name,
			m_currentFile.bytes
		);
}
