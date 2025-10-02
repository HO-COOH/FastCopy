#pragma once
#include "TestCaseBase.h"

class BigFile : public TestCaseBase
{
public:
	template<typename Path>
	BigFile(
		uint64_t totalBytes,
		Path&& source,
		Path&& destination
	) : TestCaseBase{ std::forward<Path>(source), std::forward<Path>(destination) }, m_bigFileSize{ totalBytes }
	{
	}

	void Generate() override;
private:
	uint64_t m_bigFileSize{};
};

