#pragma once
#include <string>
#include "OutputRegexParseBase.hpp"

/*
	eg:
		*EXTRA Dir        -1    F:\testDestination\FastCopy_1.0.14.0_x64_Package_Test\Add-AppDevPackage.resources\
*/
struct ExtraDir : OutputRegexParseBase<ExtraDir>
{
	constexpr static auto Prefix = "*EXTRA Dir";
	constexpr static auto regex = R"(\s*\*EXTRA Dir\s*-\d+\s*(.*)$)";

	std::string path;

	template<typename RegexMatchType>
	ExtraDir(std::match_results<RegexMatchType>&& match) : path{ match[1].str() }
	{
	}
};