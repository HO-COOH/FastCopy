#pragma once
#include <cstdio>

/**
 * @brief RAII wrapper for FILE*
 */
class FileWrapper
{
	FILE* m_fp{};
public:
	FileWrapper(FILE* fp) :m_fp{ fp } {}
	FileWrapper(FileWrapper const&) = delete;
	FileWrapper(FileWrapper&&) = delete;
	FileWrapper& operator=(FileWrapper const&) = delete;
	FileWrapper& operator=(FileWrapper&&) = delete;

	operator bool() const;

	bool read(void* buffer, size_t elementSize, size_t elementCount);

	~FileWrapper();
};

