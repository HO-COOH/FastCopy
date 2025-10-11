#include "pch.h"
#include "NewDir.h"
#include <absl/strings/str_split.h>
#include <vector>

NewDir::NewDir(std::string_view data)
{
	std::vector<std::string> splited = absl::StrSplit(data, absl::ByAnyChar{" \t"}, absl::SkipEmpty{});
	assert(splited[0] == "New" && splited[1] == "Dir");

	count = std::stoi(splited[2]);

	for (auto i = 3; i < splited.size(); ++i)
		splited[3].append(std::move(splited[i]));

	fullPath = std::move(splited[3]);
}
