#pragma once
#include <unordered_map>
#include <Windows.h>

class AnonymousPipe
{
	HANDLE m_readPipe{};
	HANDLE m_writePipe{};

public:
	AnonymousPipe();

	auto getReadHandle() const
	{
		return m_readPipe;
	}

	auto getWriteHandle() const
	{
		return m_writePipe;
	}
};
