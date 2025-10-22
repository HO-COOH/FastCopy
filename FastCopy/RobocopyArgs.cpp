#include "pch.h"
#include "RobocopyArgs.h"
#include <iostream>

RobocopyArgsBuilder::RobocopyArgsBuilder(StringView source, StringView destination, StringView builtArg) : 
	m_arg{std::format(LR"("{}" "{}" {})", source, destination, builtArg)},
	m_sourceSet{ true }
{
}

std::wstring const& RobocopyArgsBuilder::Build() const
{
	std::wcout << L"Running robocopy: " << m_arg << '\n';
	return m_arg;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Source(StringView value)
{
	if (m_destinationSet)
	{
		m_arg = std::vformat(m_arg, std::make_wformat_args(value));
	}
	else
	{
		m_arg = std::vformat(m_arg, std::make_wformat_args(value, L"{}"));
	}
	m_sourceSet = true;
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Destination(StringView value)
{
	if (m_sourceSet)
	{
		m_arg = std::vformat(m_arg, std::make_wformat_args(value));
	}
	else
	{
		m_arg = std::vformat(m_arg, std::make_wformat_args(L"{}", value));
	}
	m_destinationSet = true;
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::File(std::span<StringView> value)
{
	constexpr auto isQuoted = [](StringView filePath)
	{
		return filePath.front() == L'\"' && filePath.back() == L'\"';
	};
	for (auto file : value)
	{
		m_arg += isQuoted(file) ? std::format(L"{} ", file) : std::format(LR"("{}" )", file);
	}
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::S(bool value)
{
	return addOrRemoveFlag(L"/S ", value);
}

RobocopyArgsBuilder& RobocopyArgsBuilder::E(bool value)
{
	addOrRemoveFlag(L"/E ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LEV(int n)
{
	m_arg += std::format(L"/LEV:{} ", n);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Z(bool value)
{
	addOrRemoveFlag(L"/Z ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::B(bool value)
{
	addOrRemoveFlag(L"/B ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::ZB(bool value)
{
	addOrRemoveFlag(L"/ZB ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::J(bool value)
{
	addOrRemoveFlag(L"/J ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::EFSRAW(bool value)
{
	addOrRemoveFlag(L"/EFSRAW ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::COPY(StringView copyflags)
{
	for (auto c : copyflags)
		assert(c == L'D' || c == L'A' || c == L'T' || c == L'S' || c == L'O' || c == L'U');
	m_arg += std::format(L"/COPY:{} ", copyflags);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SEC(bool value)
{
	addOrRemoveFlag(L"/SEC ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::COPYALL(bool value)
{
	addOrRemoveFlag(L"/COPYALL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOCOPY(bool value)
{
	addOrRemoveFlag(L"/NOCOPY ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SECFIX(bool value)
{
	addOrRemoveFlag(L"/SECFIX ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TIMFIX(bool value)
{
	addOrRemoveFlag(L"/TIMFIX ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::PURGE(bool value)
{
	addOrRemoveFlag(L"/PURGE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MIR(bool value)
{
	addOrRemoveFlag(L"/MIR ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOV(bool value)
{
	addOrRemoveFlag(L"/MOV ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOVE(bool value)
{
	addOrRemoveFlag(L"/MOVE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A_plus(StringView value)
{
	m_arg += std::format(L"/A+:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A_minus(StringView value)
{
	m_arg += std::format(L"/A-:{}", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::CREATE(bool value)
{
	addOrRemoveFlag(L"/CREATE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FAT(bool value)
{
	addOrRemoveFlag(L"/FAT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::_256(bool value)
{
	addOrRemoveFlag(L"/256 ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MON(int value)
{
	m_arg += std::format(L"/MON:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOT(int value)
{
	m_arg += std::format(L"/MOT:{}", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::RH(int hh1, int mm1, int hh2, int mm2)
{
	m_arg += std::format(L"/RH:{}{}-{}{} ", hh1, mm1, hh2, mm2);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::PF(bool value)
{
	addOrRemoveFlag(L"/PF ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IPG(int ms)
{
	m_arg += std::format(L"/IPG:{} ", ms);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SL(bool value)
{
	addOrRemoveFlag(L"/SL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MT(int n)
{
	m_arg += std::format(L"/MT:{} ", n);
	return *this;
}
RobocopyArgsBuilder& RobocopyArgsBuilder::DCOPY(StringView copyFlags)
{
	for (auto c : copyFlags)
		assert(c == L'D' || c == L'A' || c == L'T');
	m_arg += std::format(L"/DCOPY:{} ", copyFlags);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NODCOPY(bool value)
{
	addOrRemoveFlag(L"/NODCOPY ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOOFFLOAD(bool value)
{
	addOrRemoveFlag(L"/NOOFFLOAD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A(bool value)
{
	addOrRemoveFlag(L"/A ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::M(bool value)
{
	addOrRemoveFlag(L"/M ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IA(StringView value)
{
	m_arg += std::format(L"/IA:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XA(StringView value)
{
	m_arg += std::format(L"/XA:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XF(std::span<StringView> file)
{
	if (file.empty())
		return *this;

	m_arg += L"/XF ";
	for (auto f : file)
		(m_arg += f) += L" ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XD(std::span<StringView> dirs)
{
	if (dirs.empty())
		return *this;

	m_arg += L"/XD ";
	for (auto f : dirs)
		(m_arg += f) += L" ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XC(bool value)
{
	addOrRemoveFlag(L"/XC ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XN(bool value)
{
	addOrRemoveFlag(L"/XN ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XO(bool value)
{
	addOrRemoveFlag(L"/XO ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XX(bool value)
{
	addOrRemoveFlag(L"/XX ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XL(bool value)
{
	addOrRemoveFlag(L"/XL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IS(bool value)
{
	addOrRemoveFlag(L"/IS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IT(bool value)
{
	addOrRemoveFlag(L"/IT ", value);
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

RobocopyArgsBuilder& RobocopyArgsBuilder::XJ(bool value)
{
	addOrRemoveFlag(L"/XJ ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FFT(bool value)
{
	addOrRemoveFlag(L"/FFT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::DST(bool value)
{
	addOrRemoveFlag(L"/DST ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJD(bool value)
{
	addOrRemoveFlag(L"/XJD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJF(bool value)
{
	addOrRemoveFlag(L"/XJF ", value);
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

RobocopyArgsBuilder& RobocopyArgsBuilder::REG(bool value)
{
	addOrRemoveFlag(L"/REG ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TBD(bool value)
{
	addOrRemoveFlag(L"/TBD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::L(bool value)
{
	addOrRemoveFlag(L"/L ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::X(bool value)
{
	addOrRemoveFlag(L"/X ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::V(bool value)
{
	addOrRemoveFlag(L"/V ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TS(bool value)
{
	addOrRemoveFlag(L"/TS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FP(bool value)
{
	addOrRemoveFlag(L"/FP ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::BYTES(bool value)
{
	addOrRemoveFlag(L"/BYTES ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NS(bool value)
{
	addOrRemoveFlag(L"/NS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NC(bool value)
{
	addOrRemoveFlag(L"/NC ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NFL(bool value)
{
	addOrRemoveFlag(L"/NFL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NDL(bool value)
{
	addOrRemoveFlag(L"/NDL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NP(bool value)
{
	addOrRemoveFlag(L"/NP ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::ETA(bool value)
{
	addOrRemoveFlag(L"/ETA ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LOG(StringView file)
{
	m_arg += std::format(L"/LOG:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LOG_plus(StringView file)
{
	m_arg += std::format(L"/LOG+:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::UNILOG(StringView file)
{
	m_arg += std::format(L"/UNILOG:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::UNILOG_Plus(StringView file)
{
	m_arg += std::format(L"/UNILOG+:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TEE(bool value)
{
	addOrRemoveFlag(L"/TEE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NJH(bool value)
{
	addOrRemoveFlag(L"/NJH ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NJS(bool value)
{
	addOrRemoveFlag(L"/NJS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Unicode(bool value)
{
	addOrRemoveFlag(L"/UNICODE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::JOB(StringView jobName)
{
	m_arg += std::format(L"/JOB:{} ", jobName);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SAVE(StringView jobName)
{
	m_arg += std::format(L"/SAVE:{} ", jobName);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::QUIT(bool value)
{
	addOrRemoveFlag(L"/QUIT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOSD(bool value)
{
	addOrRemoveFlag(L"/NOSD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NODD(bool value)
{
	addOrRemoveFlag(L"/NODD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IF(bool value)
{
	addOrRemoveFlag(L"/IF ", value);
	return *this;
}
