#pragma once

#include <string_view>

// Hands a destination folder and an already written record file to the main program
// through its fastcopy:// protocol handler. That URI is the whole contract between the
// two shell components and FastCopy.exe, so both of them launch through here.
[[nodiscard]] bool LaunchFastCopy(std::wstring_view destination, std::wstring_view recordFile);
