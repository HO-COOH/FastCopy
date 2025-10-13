#pragma once
#include <string>
#include <string_view>
#include "OutputRegexParseBase.hpp"

/*eg:
	same          1073741824      3221225472
*/
struct Same : OutputRegexParseBase<Same>
{
	constexpr static auto Prefix = "same";
	constexpr static auto regex = R"(\s*same\s*(\d+)\s*(.*)$)";

	uint64_t bytes;
	std::string name;

	template<typename MatchResultType>
	Same(std::match_results<MatchResultType>&& match) : bytes{ std::stoull(match[1].str()) }, name{ match[2].str() }
	{
	}
};
