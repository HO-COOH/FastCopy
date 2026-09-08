#include "RecordFile.h"

#include <Windows.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <format>
#include <string>

RecordFile::RecordFile(std::filesystem::path const& path) : m_file{ path, std::ios::binary }
{
}

bool RecordFile::Close()
{
    // close() on a stream that was never opened sets failbit on its own.
    if (m_file.is_open())
    {
        m_file.close();
    }
    return !m_file.fail();
}

RecordFile& RecordFile::operator<<(std::wstring_view path)
{
    // The reader wants forward slashes, and the caller's string is not ours to rewrite.
    std::wstring normalized{ path };
    std::ranges::replace(normalized, L'\\', L'/');

    // A stream that has already failed ignores these, so no guard is needed.
    auto const length = normalized.size();
    m_file.write(reinterpret_cast<char const*>(&length), sizeof(length));
    m_file.write(reinterpret_cast<char const*>(normalized.data()), length * sizeof(wchar_t));
    return *this;
}

std::wstring MakeRecordFileName(wchar_t operationFlag, std::wstring_view extension)
{
    static std::atomic_uint sequence{};
    auto const now = std::chrono::system_clock::now();
    auto const second = std::chrono::floor<std::chrono::seconds>(now);

    return std::format(L"{}{:%F_%H-%M-%S}-{:07}-{}-{}{}",
        operationFlag,
        second,
        (now - second).count(),
        GetCurrentProcessId(),
        sequence.fetch_add(1),
        extension);
}
