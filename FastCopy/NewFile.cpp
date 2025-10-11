#include "pch.h"
#include "NewFile.h"
#include <absl/strings/str_split.h>
#include <vector>

NewFile::NewFile(std::string_view data)
{
	std::vector<std::string> split = absl::StrSplit(data, "\t", absl::SkipEmpty{});
	assert(split.size() == 3 || split.size() == 4);
	bytes = std::stoull(split[1]);
	name = std::move(split[2]);
}

void NewFile::Clear() noexcept
{
	bytes = {};
	name.clear();
}
