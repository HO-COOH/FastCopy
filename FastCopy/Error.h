#pragma once
#include "OutputRegexParseBase.hpp"

/*This is a multi-line object, eg:
2026/07/29 00:00:39 ERROR 5 (0x00000005) Accessing Destination Directory E:\
Access is denied.
*/
struct Error : OutputRegexParseBase<Error>
{
	constexpr static auto regex = R"([\d\/]+ [\d:]+ ERROR .*$)";

	std::string error;

	template<typename MatchResultType>
	Error(std::match_results<MatchResultType>&& match) : error{ match[0].str() } //match[0] is the whole matched line (the regex has no capture group)
	{
	}

	//Construct a synthetic error not parsed from robocopy output, eg when the copy is interrupted by
	//the pipe being aborted after a sleep/wake.
	explicit Error(std::string message) : error{ std::move(message) }
	{
	}
};