#include "pch.h"
#include "NewDir.h"
#include <absl/strings/str_split.h>
#include <vector>

NewDir::NewDir(std::string_view data)
{
	std::vector<std::string> splited = absl::StrSplit(data, "\t");
	assert(splited.size() == 3);

	count = std::stoi(splited[1]);
	fullPath = std::move(splited[2]);
}
