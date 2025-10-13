#pragma once
#include <string>
#include "OutputRegexParseBase.hpp"

/*
	eg:
		New File  		     485	CMakeLists.txt	20:58 -> 20:58
*/

struct NewFile : public OutputRegexParseBase<NewFile>
{
	constexpr static auto Prefix = "New File";
	constexpr static auto regex = R"(\s*New File\s*(\d+)\s*(.*)\t.*$)";

	uint64_t bytes;
	std::string name;

	NewFile() = default;
	
	template<typename MatchResultType>
	NewFile(std::match_results<MatchResultType>&& match) : bytes{ std::stoull(match[1].str()) }, name{ match[2].str() }
	{
	}

	auto operator<=>(NewFile const&) const noexcept = default;
	auto operator!=(NewFile const& rhs) const noexcept { return bytes != rhs.bytes && name != rhs.name; }
	void Clear() noexcept;
};
