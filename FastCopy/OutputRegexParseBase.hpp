#pragma once
#include <regex>
#include <optional>
#include <string_view>

template<typename DerivedOutput>
struct OutputRegexParseBase
{
	static std::optional<DerivedOutput> TryParse(std::string_view data)
	{
		std::optional<DerivedOutput> ret;
		std::cmatch match;

		static std::regex regex{ DerivedOutput::regex };

		if (std::regex_match(data.data(), data.data() + data.size(), match, regex))
			ret.emplace(std::move(match));

		return ret;
	}
};
