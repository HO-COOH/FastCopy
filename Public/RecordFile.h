#pragma once

#include <filesystem>
#include <string_view>

#include <cstdio>

// The record file FastCopy reads back (see TaskFile): per path a size_t length followed
// by that many wchar_t, forward slashes throughout. Two callers write it - the shell
// extension records a selection, the keyboard hook records the clipboard - so the format
// lives here instead of being spelled out in both.
class RecordFile
{
    FILE* m_file{};
    bool m_good{};
public:
    RecordFile(std::filesystem::path const& path);
    ~RecordFile();

    RecordFile(RecordFile const&) = delete;
    RecordFile& operator=(RecordFile const&) = delete;

    // False once the open or any write has failed. Nothing is written after that.
    [[nodiscard]] bool Good() const { return m_good; }

    // Closes and reports whether everything, the final flush included, succeeded. The
    // destructor closes too, but only an explicit call can report a failed flush to a
    // caller that has to decide whether the file is worth keeping.
    bool Close();

    RecordFile& operator<<(std::wstring_view path);
};
