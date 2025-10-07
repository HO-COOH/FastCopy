#include "pch.h"
#include "RobocopyArgs.h"
#include <iostream>

RobocopyArgsBuilder::RobocopyArgsBuilder(StringView source, StringView destination, StringView builtArg) : 
	m_arg{std::format(R"("{}" "{}" {})", source, destination, builtArg)},
	m_sourceSet{ true }
{
}

std::string const& RobocopyArgsBuilder::Build() const
{
	std::cout << "Running robocopy: " << m_arg << '\n';
	return m_arg;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Source(StringView value)
{
	if (m_destinationSet)
	{
		m_arg = std::vformat(m_arg, std::make_format_args(value));
	}
	else
	{
		m_arg = std::vformat(m_arg, std::make_format_args(value, "{}"));
	}
	m_sourceSet = true;
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Destination(StringView value)
{
	if (m_sourceSet)
	{
		m_arg = std::vformat(m_arg, std::make_format_args(value));
	}
	else
	{
		m_arg = std::vformat(m_arg, std::make_format_args("{}", value));
	}
	m_destinationSet = true;
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::File(std::span<StringView> value)
{
	constexpr auto isQuoted = [](StringView filePath)
	{
		return filePath.front() == '\"' && filePath.back() == '\"';
	};
	for (auto file : value)
	{
		m_arg += isQuoted(file) ? std::format("{} ", file) : std::format(R"("{}" )", file);
	}
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::S(bool value)
{
	return addOrRemoveFlag("/S ", value);
}

RobocopyArgsBuilder& RobocopyArgsBuilder::E(bool value)
{
	addOrRemoveFlag("/E ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LEV(int n)
{
	m_arg += std::format("/LEV:{} ", n);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::Z(bool value)
{
	addOrRemoveFlag("/Z ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::B(bool value)
{
	addOrRemoveFlag("/B ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::ZB(bool value)
{
	addOrRemoveFlag("/ZB ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::J(bool value)
{
	addOrRemoveFlag("/J ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::EFSRAW(bool value)
{
	addOrRemoveFlag("/EFSRAW ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::COPY(StringView copyflags)
{
	for (auto c : copyflags)
		assert(c == 'D' || c == 'A' || c == 'T' || c == 'S' || c == 'O' || c == 'U');
	m_arg += std::format("/COPY:{} ", copyflags);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SEC(bool value)
{
	addOrRemoveFlag("/SEC ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::COPYALL(bool value)
{
	addOrRemoveFlag("/COPYALL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOCOPY(bool value)
{
	addOrRemoveFlag("/NOCOPY ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SECFIX(bool value)
{
	addOrRemoveFlag("/SECFIX ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TIMFIX(bool value)
{
	addOrRemoveFlag("/TIMFIX ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::PURGE(bool value)
{
	addOrRemoveFlag("/PURGE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MIR(bool value)
{
	addOrRemoveFlag("/MIR ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOV(bool value)
{
	addOrRemoveFlag("/MOV ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOVE(bool value)
{
	addOrRemoveFlag("/MOVE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A_plus(std::string_view value)
{
	m_arg += std::format("/A+:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A_minus(std::string_view value)
{
	m_arg += std::format("/A-:{}", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::CREATE(bool value)
{
	addOrRemoveFlag("/CREATE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FAT(bool value)
{
	addOrRemoveFlag("/FAT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::_256(bool value)
{
	addOrRemoveFlag("/256 ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MON(int value)
{
	m_arg += std::format("/MON:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MOT(int value)
{
	m_arg += std::format("/MOT:{}", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::RH(int hh1, int mm1, int hh2, int mm2)
{
	m_arg += std::format("/RH:{}{}-{}{} ", hh1, mm1, hh2, mm2);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::PF(bool value)
{
	addOrRemoveFlag("/PF ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IPG(int ms)
{
	m_arg += std::format("/IPG:{} ", ms);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SL(bool value)
{
	addOrRemoveFlag("/SL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MT(int n)
{
	m_arg += std::format("/MT:{} ", n);
	return *this;
}
RobocopyArgsBuilder& RobocopyArgsBuilder::DCOPY(StringView copyFlags)
{
	for (auto c : copyFlags)
		assert(c == 'D' || c == 'A' || c == 'T');
	m_arg += std::format("/DCOPY:{} ", copyFlags);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NODCOPY(bool value)
{
	addOrRemoveFlag("/NODCOPY ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOOFFLOAD(bool value)
{
	addOrRemoveFlag("/NOOFFLOAD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::A(bool value)
{
	addOrRemoveFlag("/A ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::M(bool value)
{
	addOrRemoveFlag("/M ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IA(StringView value)
{
	m_arg += std::format("/IA:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XA(StringView value)
{
	m_arg += std::format("/XA:{} ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XF(std::span<StringView> file)
{
	if (file.empty())
		return *this;

	m_arg += "/XF ";
	for (auto f : file)
		(m_arg += f) += " ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XD(std::span<StringView> dirs)
{
	if (dirs.empty())
		return *this;

	m_arg += "/XD ";
	for (auto f : dirs)
		(m_arg += f) += " ";
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XC(bool value)
{
	addOrRemoveFlag("/XC ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XN(bool value)
{
	addOrRemoveFlag("/XN ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XO(bool value)
{
	addOrRemoveFlag("/XO ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XX(bool value)
{
	addOrRemoveFlag("/XX ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XL(bool value)
{
	addOrRemoveFlag("/XL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IS(bool value)
{
	addOrRemoveFlag("/IS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IT(bool value)
{
	addOrRemoveFlag("/IT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MAX(size_t bytes)
{
	m_arg += std::format("/MAX:{} ", bytes);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MIN(size_t bytes)
{
	m_arg += std::format("/MIN:{} ", bytes);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MAXAGE(size_t days)
{
	m_arg += std::format("/MAXAGE:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MINAGE(size_t days)
{
	m_arg += std::format("/MINAGE:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MAXLAD(size_t days)
{
	m_arg += std::format("/MAXLAD:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::MINLAD(size_t days)
{
	m_arg += std::format("/MINLAD:{} ", days);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJ(bool value)
{
	addOrRemoveFlag("/XJ ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FFT(bool value)
{
	addOrRemoveFlag("/FFT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::DST(bool value)
{
	addOrRemoveFlag("/DST ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJD(bool value)
{
	addOrRemoveFlag("/XJD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::XJF(bool value)
{
	addOrRemoveFlag("/XJF ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::R(size_t n)
{
	m_arg += std::format("/R:{} ", n);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::W(size_t seconds)
{
	m_arg += std::format("/W:{} ", seconds);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::REG(bool value)
{
	addOrRemoveFlag("/REG ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TBD(bool value)
{
	addOrRemoveFlag("/TBD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::L(bool value)
{
	addOrRemoveFlag("/L ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::X(bool value)
{
	addOrRemoveFlag("/X ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::V(bool value)
{
	addOrRemoveFlag("/V ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TS(bool value)
{
	addOrRemoveFlag("/TS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::FP(bool value)
{
	addOrRemoveFlag("/FP ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::BYTES(bool value)
{
	addOrRemoveFlag("/BYTES ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NS(bool value)
{
	addOrRemoveFlag("/NS ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NC(bool value)
{
	addOrRemoveFlag("/NC ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NFL(bool value)
{
	addOrRemoveFlag("/NFL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NDL(bool value)
{
	addOrRemoveFlag("/NDL ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NP(bool value)
{
	addOrRemoveFlag("/NP ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::ETA(bool value)
{
	addOrRemoveFlag("/ETA ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LOG(StringView file)
{
	m_arg += std::format("/LOG:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::LOG_plus(StringView file)
{
	m_arg += std::format("/LOG+:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::UNILOG(StringView file)
{
	m_arg += std::format("/UNILOG:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::UNILOG_Plus(StringView file)
{
	m_arg += std::format("/UNILOG+:{} ", file);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::TEE(bool value)
{
	addOrRemoveFlag("/TEE ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NJH(bool value)
{
	addOrRemoveFlag("/NJH ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NJS(bool value)
{
	addOrRemoveFlag("/NJS ", value);
	return *this;
}

#ifndef UNICODE
RobocopyArgsBuilder& RobocopyArgsBuilder::UNICODE(bool value)
{
	addOrRemoveFlag("/UNICODE ", value);
	return *this;
}
#endif

RobocopyArgsBuilder& RobocopyArgsBuilder::JOB(StringView jobName)
{
	m_arg += std::format("/JOB:{} ", jobName);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::SAVE(StringView jobName)
{
	m_arg += std::format("/SAVE:{} ", jobName);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::QUIT(bool value)
{
	addOrRemoveFlag("/QUIT ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NOSD(bool value)
{
	addOrRemoveFlag("/NOSD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::NODD(bool value)
{
	addOrRemoveFlag("/NODD ", value);
	return *this;
}

RobocopyArgsBuilder& RobocopyArgsBuilder::IF(bool value)
{
	addOrRemoveFlag("/IF ", value);
	return *this;
}
