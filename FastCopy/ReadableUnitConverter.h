#pragma once
#include <string>
#include <format>
namespace ReadableUnitConverter
{
	class Size
	{
		template<typename Char>
		struct FormatString;

		template<>
		struct FormatString<char>
		{
			constexpr static auto B = "{} B";
			constexpr static auto KB = "{} KB";
			constexpr static auto MB = "{} MB";
			constexpr static auto GB = "{} GB";
		};

		template<>
		struct FormatString<wchar_t>
		{
			constexpr static auto B = L"{} B";
			constexpr static auto KB = L"{} KB";
			constexpr static auto MB = L"{} MB";
			constexpr static auto GB = L"{} GB";
		};

		constexpr static size_t ToKB = 1024;
		constexpr static size_t ToMB = ToKB * 1024;
		constexpr static size_t ToGB = ToMB * 1024;
	public:
		template<typename Char>
		static auto ToString(size_t bytes)
		{
			if (bytes <= ToKB)
				return std::format(FormatString<Char>::B, bytes);
			else if (bytes <= ToMB)
				return std::format(FormatString<Char>::KB, bytes / ToKB);
			else if (bytes <= ToGB)
				return std::format(FormatString<Char>::MB, bytes / ToMB);
			else return std::format(FormatString<Char>::GB, bytes / ToGB);
		}

	};

	struct Speed
	{
		template<typename Char>
		static auto ToString(size_t bytes, std::chrono::nanoseconds duration)
		{
			auto const bytesPerSec = bytes / (static_cast<double>(duration.count()) / 1e9);
			return Size::ToString<Char>(bytesPerSec) + L"/s";
		}
	};
}

