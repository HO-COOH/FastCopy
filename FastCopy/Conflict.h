#pragma once
#include <array>
#include <string>
#include "OutputRegexParseBase.hpp"

/*
	eg:
		older                16        New document.txt
*/
struct Conflict : OutputRegexParseBase<Conflict>
{
	constexpr static std::array Prefix{ "older", "newer"};
	constexpr static auto regex{ R"(\s*((older)|(newer))\s*(\d+)\s*(.*)$)" };

	uint64_t bytes;
	std::string name;

	template<typename MatchResultType>
	Conflict(std::match_results<MatchResultType>&& match) : bytes{ std::stoull(match[4].str()) }, name{ match[5].str() }
	{
	}
};
