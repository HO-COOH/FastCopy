#include "pch.h"
#include "RobocopyArgs.h"
#include <iostream>

static void appendEscapedBraces(std::wstring& out, std::wstring_view str)
{
	for (wchar_t const ch : str)
	{
		out.push_back(ch);
		if (ch == L'{' || ch == L'}')
			out.push_back(ch);
	}
}

static auto sanitizeQuotedDirArg(std::wstring_view dir, bool escapeBraces)
{
	// robocopy uses CommandLineToArgvW rules where a trailing backslash right before a closing quote
	// (such as a drive root "E:\") escapes the quote unless doubled ("E:\\") or stripped.
	std::wstring result;
	result.reserve(dir.size() + 2);
	if (escapeBraces)
		appendEscapedBraces(result, dir);
	else
		result.append(dir);

	if (dir.ends_with(L'\\'))
		result.push_back(L'\\');
	return result;
}

std::wstring const& RobocopyArgsBuilder::Build() const
{
	std::wcout << L"robocopy " << m_arg << L'\n';
	assert(m_sourceSet && m_destinationSet);
	return m_arg;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Source(std::wstring_view value)
{
	auto const sanitized = sanitizeQuotedDirArg(value, !m_destinationSet);
	m_arg = m_destinationSet
		? std::vformat(m_arg, std::make_wformat_args(sanitized))
		: std::vformat(m_arg, std::make_wformat_args(sanitized, L"{}"));
	m_sourceSet = true;
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Destination(std::wstring_view value)
{
	auto const sanitized = sanitizeQuotedDirArg(value, !m_sourceSet);
	m_arg = m_sourceSet
		? std::vformat(m_arg, std::make_wformat_args(sanitized))
		: std::vformat(m_arg, std::make_wformat_args(L"{}", sanitized));
	m_destinationSet = true;
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::File(std::span<std::wstring_view> value)
{
	constexpr auto isQuoted = [](std::wstring_view filePath)
	{
		return filePath.size() >= 2 && filePath.front() == L'\"' && filePath.back() == L'\"';
	};
	for (auto file : value)
	{
		m_arg += isQuoted(file) ? std::format(L"{} ", file) : std::format(LR"("{}" )", file);
	}
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::S()
{
	m_arg += L"/S ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::E()
{
	m_arg += L"/E ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LEV(int n)
{
	m_arg += std::format(L"/LEV:{} ", n);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Z()
{
	m_arg += L"/Z ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::B()
{
	m_arg += L"/B ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::ZB()
{
	m_arg += L"/ZB ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::J()
{
	m_arg += L"/J ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::EFSRAW()
{
	m_arg += L"/EFSRAW ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::COPY(std::wstring_view copyflags)
{
	for ([[maybe_unused]] auto c : copyflags)
		assert(c == L'D' || c == L'A' || c == L'T' || c == L'S' || c == L'O' || c == L'U');
	m_arg += std::format(L"/COPY:{} ", copyflags);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SEC()
{
	m_arg += L"/SEC ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::COPYALL()
{
	m_arg += L"/COPYALL ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOCOPY()
{
	m_arg += L"/NOCOPY ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SECFIX()
{
	m_arg += L"/SECFIX ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TIMFIX()
{
	m_arg += L"/TIMFIX ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::PURGE()
{
	m_arg += L"/PURGE ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MIR()
{
	m_arg += L"/MIR ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOV()
{
	m_arg += L"/MOV ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOVE()
{
	m_arg += L"/MOVE ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A_plus(std::wstring_view value)
{
	m_arg += std::format(L"/A+:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A_minus(std::wstring_view value)
{
	m_arg += std::format(L"/A-:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::CREATE()
{
	m_arg += L"/CREATE ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FAT()
{
	m_arg += L"/FAT ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::_256()
{
	m_arg += L"/256 ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MON(int value)
{
	m_arg += std::format(L"/MON:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOT(int value)
{
	m_arg += std::format(L"/MOT:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::RH(int hh1, int mm1, int hh2, int mm2)
{
	m_arg += std::format(L"/RH:{}{}-{}{} ", hh1, mm1, hh2, mm2);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::PF()
{
	m_arg += L"/PF ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IPG(int ms)
{
	m_arg += std::format(L"/IPG:{} ", ms);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SL()
{
	m_arg += L"/SL ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MT(int n)
{
	m_arg += std::format(L"/MT:{} ", n);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::DCOPY(std::wstring_view copyFlags)
{
	for ([[maybe_unused]] auto c : copyFlags)
		assert(c == L'D' || c == L'A' || c == L'T');
	m_arg += std::format(L"/DCOPY:{} ", copyFlags);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NODCOPY()
{
	m_arg += L"/NODCOPY ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOOFFLOAD()
{
	m_arg += L"/NOOFFLOAD ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A()
{
	m_arg += L"/A ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::M()
{
	m_arg += L"/M ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IA(std::wstring_view value)
{
	m_arg += std::format(L"/IA:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XA(std::wstring_view value)
{
	m_arg += std::format(L"/XA:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XF(std::span<std::wstring_view> file)
{
	if (file.empty())
		return *this;

	m_arg += L"/XF ";
	for (auto f : file)
		(m_arg += f) += L" ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XD(std::span<std::wstring_view> dirs)
{
	if (dirs.empty())
		return *this;

	m_arg += L"/XD ";
	for (auto f : dirs)
		(m_arg += f) += L" ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XC()
{
	m_arg += L"/XC ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XN()
{
	m_arg += L"/XN ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XO()
{
	m_arg += L"/XO ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XX()
{
	m_arg += L"/XX ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XL()
{
	m_arg += L"/XL ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IS()
{
	m_arg += L"/IS ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IT()
{
	m_arg += L"/IT ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MAX(size_t bytes)
{
	m_arg += std::format(L"/MAX:{} ", bytes);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MIN(size_t bytes)
{
	m_arg += std::format(L"/MIN:{} ", bytes);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MAXAGE(size_t days)
{
	m_arg += std::format(L"/MAXAGE:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MINAGE(size_t days)
{
	m_arg += std::format(L"/MINAGE:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MAXLAD(size_t days)
{
	m_arg += std::format(L"/MAXLAD:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MINLAD(size_t days)
{
	m_arg += std::format(L"/MINLAD:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJ()
{
	m_arg += L"/XJ ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FFT()
{
	m_arg += L"/FFT ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::DST()
{
	m_arg += L"/DST ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJD()
{
	m_arg += L"/XJD ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJF()
{
	m_arg += L"/XJF ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::R(size_t n)
{
	m_arg += std::format(L"/R:{} ", n);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::W(size_t seconds)
{
	m_arg += std::format(L"/W:{} ", seconds);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::REG()
{
	m_arg += L"/REG ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TBD()
{
	m_arg += L"/TBD ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::L()
{
	m_arg += L"/L ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::X()
{
	m_arg += L"/X ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::V()
{
	m_arg += L"/V ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TS()
{
	m_arg += L"/TS ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FP()
{
	m_arg += L"/FP ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::BYTES()
{
	m_arg += L"/BYTES ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NS()
{
	m_arg += L"/NS ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NC()
{
	m_arg += L"/NC ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NFL()
{
	m_arg += L"/NFL ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NDL()
{
	m_arg += L"/NDL ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NP()
{
	m_arg += L"/NP ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::ETA()
{
	m_arg += L"/ETA ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LOG(std::wstring_view file)
{
	m_arg += std::format(L"/LOG:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LOG_plus(std::wstring_view file)
{
	m_arg += std::format(L"/LOG+:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::UNILOG(std::wstring_view file)
{
	m_arg += std::format(L"/UNILOG:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::UNILOG_Plus(std::wstring_view file)
{
	m_arg += std::format(L"/UNILOG+:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TEE()
{
	m_arg += L"/TEE ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NJH()
{
	m_arg += L"/NJH ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NJS()
{
	m_arg += L"/NJS ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Unicode()
{
	m_arg += L"/UNICODE ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::JOB(std::wstring_view jobName)
{
	constexpr auto isQuoted = [](std::wstring_view path)
	{
		return path.size() >= 2 && path.front() == L'\"' && path.back() == L'\"';
	};
	m_arg += isQuoted(jobName) ? std::format(L"/JOB:{} ", jobName) : std::format(LR"(/JOB:"{}" )", jobName);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SAVE(std::wstring_view jobName)
{
	constexpr auto isQuoted = [](std::wstring_view path)
	{
		return path.size() >= 2 && path.front() == L'\"' && path.back() == L'\"';
	};
	m_arg += isQuoted(jobName) ? std::format(L"/SAVE:{} ", jobName) : std::format(LR"(/SAVE:"{}" )", jobName);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::QUIT()
{
	m_arg += L"/QUIT ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOSD()
{
	m_arg += L"/NOSD ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NODD()
{
	m_arg += L"/NODD ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IF()
{
	m_arg += L"/IF ";
	return *this;
}
