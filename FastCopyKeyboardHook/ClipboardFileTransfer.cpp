#include "ClipboardFileTransfer.h"
#include "ClipboardFormat.h"
#include "AppFolders.h"
#include "FastCopyLauncher.h"
#include "RecordFile.h"
#include <wil/resource.h>
#include <ShlObj_core.h>
#include <algorithm>
#include <atomic>
#include <cstddef>
#include <format>

static std::optional<DWORD> GetPreferredDropEffect()
{
    ClipboardFormat const effect{ RegisterClipboardFormatW(CFSTR_PREFERREDDROPEFFECT) };
    if (!effect || effect.size() < sizeof(DWORD))
    {
        return std::nullopt;
    }
    return *reinterpret_cast<DWORD const*>(effect.data());
}

static std::optional<std::vector<std::wstring>> ReadDropFiles()
{
    ClipboardFormat const drop{ CF_HDROP };
    if (!drop)
    {
        return std::nullopt;
    }

    // GlobalSize is the only bound available: the header carries neither a file count
    // nor a list length, and the trailing empty string is the sole documented terminator.
    auto const base = drop.data();
    auto const size = drop.size();
    if (size < sizeof(DROPFILES))
    {
        return std::nullopt;
    }

    // pFiles is a byte offset rather than a fixed sizeof(DROPFILES), so a producer may
    // pad between the header and the list. Explorer always writes wide, in-bounds and
    // aligned offsets, but the clipboard holds data from any process and none of these
    // properties are documented as guaranteed, so anything that fails to add up is
    // refused instead of interpreted.
    auto const& header = *reinterpret_cast<DROPFILES const*>(base);
    if (!header.fWide ||
        header.pFiles < sizeof(DROPFILES) ||
        header.pFiles > size ||
        header.pFiles % sizeof(wchar_t) != 0)
    {
        return std::nullopt;
    }

    // A run of null-terminated paths ended by an extra null. Counting whole wchar_t from
    // the list start is the bounds validation DragQueryFileW used to do for us, and the
    // single forward walk is what it could not: it restarts from the head on every call,
    // which made reading N paths quadratic.
    auto const* cursor = reinterpret_cast<wchar_t const*>(base + header.pFiles);
    auto const* const end = cursor + (size - header.pFiles) / sizeof(wchar_t);

    std::vector<std::wstring> paths;
    while (cursor < end && *cursor)
    {
        auto const* const terminator = std::find(cursor, end, L'\0');
        if (terminator == end)
        {
            // The last path runs to the end of the block with no terminator in sight.
            return std::nullopt;
        }
        paths.emplace_back(cursor, terminator);
        cursor = terminator + 1;
    }

    return paths.empty() ? std::nullopt : std::optional{ std::move(paths) };
}

std::optional<ClipboardFileTransfer> ClipboardFileTransfer::Read()
{
    // Reading needs no owner window. Another process holding the clipboard open fails
    // this outright, which is why the paths are copied out of the locked blocks and into
    // the vector before the scope exit closes it again.
    if (!OpenClipboard(nullptr))
    {
        return std::nullopt;
    }
    auto const closeClipboard = wil::scope_exit([] { CloseClipboard(); });

    auto paths = ReadDropFiles();
    if (!paths)
    {
        return std::nullopt;
    }

    ClipboardFileTransfer transfer{ .paths = std::move(*paths) };
    if (auto const effect = GetPreferredDropEffect())
    {
        auto const value = *effect;
        transfer.move = (value & DROPEFFECT_MOVE) && !(value & DROPEFFECT_COPY);
    }

    return transfer;
}

static std::optional<std::filesystem::path> WriteRecordFile(ClipboardFileTransfer const& transfer)
{
    FILETIME timestamp{};
    GetSystemTimePreciseAsFileTime(&timestamp);
    ULARGE_INTEGER const timestampValue
    {
        .LowPart = timestamp.dwLowDateTime,
        .HighPart = timestamp.dwHighDateTime
    };
    static std::atomic_uint sequence{};

    // The hook writes into its own folder: Recorder::HasRecord scans the shell
    // extension's, and a record left behind there would keep the Paste entry showing.
    auto const path = AppFolders::ClipboardRecordsFolder(true) / std::format(
        L"{}{}-{}-{}.bin",
        transfer.move ? L'M' : L'C',
        timestampValue.QuadPart,
        GetCurrentProcessId(),
        sequence.fetch_add(1));

    RecordFile record{ path };
    for (auto const& source : transfer.paths)
    {
        record << source;
    }
    if (record.Close())
    {
        return path;
    }

    std::error_code error;
    std::filesystem::remove(path, error);
    return std::nullopt;
}

bool ClipboardFileTransfer::Paste(std::filesystem::path const& destination) const
{
    auto const record = WriteRecordFile(*this);
    if (!record)
    {
        return false;
    }

    if (LaunchFastCopy(destination.native(), record->native()))
    {
        return true;
    }

    std::error_code error;
    std::filesystem::remove(*record, error);
    return false;
}
