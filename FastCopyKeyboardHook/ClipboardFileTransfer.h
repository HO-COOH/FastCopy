#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct ClipboardFileTransfer
{
    std::vector<std::wstring> paths;
    bool move{};

    static std::optional<ClipboardFileTransfer> Read();

    // Writes the transfer to a record file and hands it to FastCopy. A record whose
    // launch fails is removed again, so a failed paste leaves nothing behind for the
    // next one to find.
    [[nodiscard]] bool Paste(std::filesystem::path const& destination) const;
};
