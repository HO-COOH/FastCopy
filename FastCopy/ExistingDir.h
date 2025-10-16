#pragma once
#include "OutputRegexParseBase.hpp"

/*eg:
	4   F:\myDir
*/
struct ExistingDir : OutputRegexParseBase<ExistingDir>
{
	constexpr static auto regex = R"(\s*(\d+)\s+(.*)$)";

	int count;
	std::string name;

	template<typename MatchResultType>
	ExistingDir(std::match_results<MatchResultType>&& match) : count{ std::stoi(match[1].str()) }, name{ match[2].str() }
	{
	}
};
