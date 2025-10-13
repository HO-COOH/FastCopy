#pragma once
#include <string>
#include "OutputRegexParseBase.hpp"
/*eg:  
	New Dir          2    F : \testSource\subFolder
*/
struct NewDir : OutputRegexParseBase<NewDir>
{
	constexpr static auto Prefix = "New Dir";
	constexpr static auto regex = R"(\s*New Dir\s*(\d+)\s*(.*)$)";

	int count;
	std::string fullPath;

	NewDir() = default;

	template<typename RegexMatchType>
	NewDir(std::match_results<RegexMatchType>&& match) : count{std::stoi(match[1].str())}, fullPath{match[2].str()}
	{
	}
};
