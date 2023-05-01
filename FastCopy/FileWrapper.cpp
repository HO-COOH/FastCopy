#include "pch.h"
#include "FileWrapper.h"

FileWrapper::operator bool() const
{
    return m_fp != nullptr;
}

bool FileWrapper::read(void* buffer, size_t elementSize, size_t elementCount)
{
    return fread(buffer, elementSize, elementCount, m_fp) == elementCount;
}

FileWrapper::~FileWrapper()
{
    if (m_fp)
        fclose(m_fp);
}
