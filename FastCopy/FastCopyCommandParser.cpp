#include "pch.h"
#include "FastCopyCommandParser.h"

AnonymousPipe::AnonymousPipe()
{
	SECURITY_ATTRIBUTES saAttr;
	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	auto success = CreatePipe(&m_readPipe, &m_writePipe, &saAttr, 0);
	assert(success);

	success = SetHandleInformation(m_readPipe, HANDLE_FLAG_INHERIT, 0);
	assert(success);

}

static std::pair<std::string, std::string> split(std::string_view line)
{
	auto pos = line.find(" ");
	auto s1 = line.substr(0, pos);
	std::string_view s2 = pos == std::string::npos ? "" : line.substr(line.find_first_not_of(" ", pos));
	return { std::string{s1}, s2.empty() ? std::string{""} : std::string{s2} };
}

// trim from start (in place)
static inline void ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s)
{
	rtrim(s);
	ltrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

std::string FastCopyCommandParser::runHelp(Command command)
{
	AnonymousPipe pipe;

	STARTUPINFOA si
	{
		.cb = sizeof(si),
		.dwFlags = STARTF_USESTDHANDLES,
		.hStdOutput = pipe.getWriteHandle(),
	};
	PROCESS_INFORMATION pi{};

	char cmd[MAX_PATH]{};
	switch (command)
	{
		case Command::xcopy: strcpy_s(cmd, R"(C:\Windows\system32\xcopy.exe /?)"); break;
		case Command::robocopy: strcpy_s(cmd, R"(C:\Windows\system32\Robocopy.exe)"); break;
		default: throw std::exception{ "Not implmented command" };
	}

	auto success = CreateProcessA(
		nullptr, 
		cmd, 
		nullptr, 
		nullptr,
		true, 
		CREATE_NO_WINDOW, 
		nullptr,
		nullptr, 
		&si,
		&pi);

	assert(success);
	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pipe.getWriteHandle());
	CHAR buf[10000]{};
	DWORD dwRead{};
	ReadFile(pipe.getReadHandle(), buf, 10000, &dwRead, nullptr);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return buf;
}

void FastCopyCommandParser::parseHelp(std::string const& helpString)
{
	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	auto delimiter = '\n';

	auto parseLine = [this](std::string const& line)
	{
		if (line.empty())
		{
			iter = m_switches.end();
			return;
		}
		auto type = DetermineLineType(line);
		switch (type)
		{
			case FastCopyCommandParser::LineType::Empty:
				iter = m_switches.end();
				break;
			case FastCopyCommandParser::LineType::FlagAndDescription:
			{
				auto stripped = line.substr(line.find_first_not_of(" "));
				auto [flag, description] = split(stripped);
				trim(flag);
				trim(description);
				m_switches[flag] += description;
				iter = m_switches.find(flag);
				break;
			}
			case FastCopyCommandParser::LineType::Description:
				if (iter != m_switches.end())
				{
					iter->second += trim_copy(line);;
				}
				break;
			default:
				break;
		}
	};

	while ((pos = helpString.find(delimiter, prev)) != std::string::npos)
	{
		auto line = helpString.substr(prev, pos - prev);
		parseLine(line);
		prev = pos + sizeof(delimiter);
	}
}