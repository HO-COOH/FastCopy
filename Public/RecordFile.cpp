#include "RecordFile.h"

#include <algorithm>
#include <string>

RecordFile::RecordFile(std::filesystem::path const& path)
{
    m_good = _wfopen_s(&m_file, path.c_str(), L"wb") == 0 && m_file != nullptr;
}

RecordFile::~RecordFile()
{
    Close();
}

bool RecordFile::Close()
{
    if (m_file)
    {
        m_good = fclose(m_file) == 0 && m_good;
        m_file = nullptr;
    }
    return m_good;
}

RecordFile& RecordFile::operator<<(std::wstring_view path)
{
    if (!m_good)
    {
        return *this;
    }

    // The reader wants forward slashes, and the caller's string is not ours to rewrite.
    std::wstring normalized{ path };
    std::ranges::replace(normalized, L'\\', L'/');

    auto const length = normalized.size();
    m_good = fwrite(&length, sizeof(length), 1, m_file) == 1 &&
        fwrite(normalized.data(), sizeof(wchar_t), length, m_file) == length;
    return *this;
}
