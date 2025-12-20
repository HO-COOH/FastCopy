#pragma once
#include "OutputRegexParseBase.hpp"

/*
	eg:
		*EXTRA File              39434        Add-AppDevPackage.psd1
*/
struct ExtraFile : OutputRegexParseBase<ExtraFile>
{
	constexpr static auto Prefix = "*EXTRA File";
	constexpr static auto regex = R"(\*EXTRA File\s*(\d+)\s*(.*)$)";

	uint64_t bytes;
	std::string name;

	template<typename MatchResultType>
	ExtraFile(std::match_results<MatchResultType>&& match) : bytes{ std::stoull(match[1].str()) }, name{ match[2].str() }
	{
	}
};