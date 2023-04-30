#pragma once
#include <string>
#include <vector>

/**
 * @brief Representing a command line switch.
 * @details When the flag is specified, output {name}. Otherwise outputs nothing
 */
struct Flag
{
	std::wstring_view name;
	bool value{ false };

	constexpr operator std::wstring() const
	{
		return value ? std::format(L"{} ", name) : L"";
	}
};

/**
 * @brief Representing a command line variable.
 * @details When the variable is specified, output {name}:{value}
 */
template<typename ValueType>
struct Variable
{
	std::wstring_view name;
	std::optional<ValueType> value;

	operator std::wstring() const
	{
		return value.has_value() ? std::format(L"{}:{} ", name, *value) : L"";
	}
};

/**
 * @brief Represents arguments that robocopy.exe accepts
 */
struct RobocopyArgs
{
	std::wstring sourceDir;
	std::wstring destinationDir;

	std::vector<std::wstring> files;

	/*options*/
	Flag copySubDirectoriesExceptEmpty{ L"/S" };
	Flag copySubDirectoriesIncludeEmpty{ L"/E" };
	Flag moveFiles{ L"/MOV" };
	Flag moveFilesAndDirs{ L"/MOVE" };
	Flag unbufferedIO{ L"/J" };

	Variable<int> multiThread{ L"/MT", 32 };


	std::wstring ToString() const;
};
