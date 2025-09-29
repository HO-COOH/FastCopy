#pragma once
#include "TestCaseBase.h"


class Random4KFiles : public TestCaseBase
{
public:
	template<typename Path>
	Random4KFiles(
		uint64_t totalBytes,
		Path&& source,
		Path&& destination
	) : TestCaseBase{ std::forward<Path>(source), std::forward<Path>(destination) }, m_totalBytes{ totalBytes }
	{
	}

	void Generate();
private:
	uint64_t m_totalBytes{};

	constexpr static auto RandomFileSize = 4 * 1024; //4K
};

