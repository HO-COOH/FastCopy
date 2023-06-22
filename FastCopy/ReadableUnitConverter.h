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
			constexpr static auto KB = "{:.2f} KB";
			constexpr static auto MB = "{:.2f} MB";
			constexpr static auto GB = "{:.2f} GB";
		};

		template<>
		struct FormatString<wchar_t>
		{
			constexpr static auto B = L"{} B";
			constexpr static auto KB = L"{:.2f} KB";
			constexpr static auto MB = L"{:.2f} MB";
			constexpr static auto GB = L"{:.2f} GB";
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
				return std::format(FormatString<Char>::KB, static_cast<double>(bytes) / ToKB);
			else if (bytes <= ToGB)
				return std::format(FormatString<Char>::MB, static_cast<double>(bytes) / ToMB);
			else return std::format(FormatString<Char>::GB, static_cast<double>(bytes) / ToGB);
		}

	};

	struct Speed
	{
		static auto BytesPerSec(size_t bytes, std::chrono::nanoseconds duration)
		{
			return duration.count() == 0 ? 0 : bytes / (static_cast<double>(duration.count()) / 1e9);
		}

		template<typename Char>
		static auto ToString(size_t bytes, std::chrono::nanoseconds duration)
		{
			if (duration.count() == 0)
			{
				if constexpr (std::is_same_v<Char, char>)
				{
					return decltype(Size::ToString<char>(1)){"---"};
				}
				else
				{
					return decltype(Size::ToString<wchar_t>(1)) {L"---"};
				}
			}
			return Size::ToString<Char>(BytesPerSec(bytes, duration)) + L"/s";
		}

		template<typename Char>
		static auto ToString(double bytesPerSec)
		{
			if (bytesPerSec == 0)
			{
				if constexpr (std::is_same_v<Char, char>)
				{
					return decltype(Size::ToString<char>(1)){"---"};
				}
				else
				{
					return decltype(Size::ToString<wchar_t>(1)) {L"---"};
				}
			}
			return Size::ToString<Char>(bytesPerSec) + L"/s";
		}
	};
}

