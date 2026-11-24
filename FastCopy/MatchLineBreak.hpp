#pragma once
#include <utility>

struct MatchLineBreak
{
	using result_type = bool; // Required by boost::asio::is_match_condition

	template <typename Iterator>
	constexpr std::pair<Iterator, bool> operator()(Iterator begin, Iterator end) const noexcept
	{
		auto const it = std::find_if(begin, end, [](char ch) 
		{
			return ch == '\r' || ch == '\n';
		});
		return it != end ? std::pair{ std::next(it), true } : std::pair{ end, false };
	}
};
