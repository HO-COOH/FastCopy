#pragma once
#include <unordered_map>
#include <Windows.h>

class AnonymousPipe
{
	HANDLE m_readPipe{};
	HANDLE m_writePipe{};

public:
	AnonymousPipe();

	auto getReadHandle() const
	{
		return m_readPipe;
	}

	auto getWriteHandle() const
	{
		return m_writePipe;
	}
};

class FastCopyCommandParser
{
	std::unordered_map<std::string, std::string> m_switches;
	std::unordered_map<std::string, std::string>::iterator iter = m_switches.end();
public:

	enum class Command
	{
		xcopy,
		robocopy
	};

	std::string runHelp(Command command);

	enum class LineType
	{
		Empty,
		FlagAndDescription,
		Description
	};

	static LineType DetermineLineType(std::string_view line)
	{
		auto stripped = line.substr(line.find_first_not_of(" "));
		if (stripped.starts_with("/"))
			return LineType::FlagAndDescription;
		else if (stripped.empty())
			return LineType::Empty;
		return LineType::Description;
	}


	void parseHelp(std::string const& helpString);

	std::unordered_map<std::string, std::string> const& getSwitches() const
	{
		return m_switches;
	}
};

