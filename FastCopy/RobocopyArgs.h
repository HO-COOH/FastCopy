#pragma once
#include <string>
#include <vector>
#include <span>
#include <boost/algorithm/string.hpp>
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


class RobocopyArgsBuilder
{
	std::string m_arg = R"("{}" "{}" )";
	bool m_sourceSet = false;
	bool m_destinationSet = false;


public:
	using StringView = std::string_view;

	constexpr RobocopyArgsBuilder& addOrRemoveFlag(StringView name, bool value)
	{
		if (value)
			m_arg += name;
		else
			boost::algorithm::erase_all(m_arg, name);
		return *this;
	}

	enum class CopyFlags
	{
		D = 'D',
		A = 'A',
		T = 'T',
		S = 'S',
		O = 'O',
		U = 'U'
	};
	RobocopyArgsBuilder() = default;

	RobocopyArgsBuilder(StringView source, StringView destination, StringView builtArg);

	std::string const& Build() const;

	//Source Directory (drive:\path or \\server\share\path)
	RobocopyArgsBuilder& Source(StringView value);

	//Destination Dir  (drive:\path or \\server\share\path).
	RobocopyArgsBuilder& Destination(StringView value);

	//File(s) to copy  (names/wildcards: default is "*.*").
	RobocopyArgsBuilder& File(std::span<StringView> value);

#pragma region CopyOptions
	//copy Subdirectories, but not empty ones.
	RobocopyArgsBuilder& S(bool value);

	//copy subdirectories, including Empty ones.
	RobocopyArgsBuilder& E(bool value);

	//only copy the top n LEVels of the source directory tree
	RobocopyArgsBuilder& LEV(int n);

	//copy files in restartable mode.
	RobocopyArgsBuilder& Z(bool value);

	//copy files in Backup mode.
	RobocopyArgsBuilder& B(bool value);

	//use restartable mode; if access denied use Backup mode.
	RobocopyArgsBuilder& ZB(bool value);

	//copy using unbuffered I/O (recommended for large files).
	RobocopyArgsBuilder& J(bool value);

	//copy all encrypted files in EFS RAW mode.
	RobocopyArgsBuilder& EFSRAW(bool value);

	/*
	* what to COPY for files (default is /COPY:DAT).
                       (copyflags : D=Data, A=Attributes, T=Timestamps).
                       (S=Security=NTFS ACLs, O=Owner info, U=aUditing info).
	*/
	RobocopyArgsBuilder& COPY(StringView copyflags);

	//copy files with SECurity (equivalent to /COPY:DATS).
	RobocopyArgsBuilder& SEC(bool value);

	//COPY ALL file info (equivalent to /COPY:DATSOU).
	RobocopyArgsBuilder& COPYALL(bool value);

	//COPY NO file info (useful with /PURGE).
	RobocopyArgsBuilder& NOCOPY(bool value);

	//FIX file SECurity on all files, even skipped files.
	RobocopyArgsBuilder& SECFIX(bool value);

	//FIX file TIMes on all files, even skipped files.
	RobocopyArgsBuilder& TIMFIX(bool value);

	//delete dest files/dirs that no longer exist in source.
	RobocopyArgsBuilder& PURGE(bool value);

	//MIRror a directory tree (equivalent to /E plus /PURGE).
	RobocopyArgsBuilder& MIR(bool value);

	//MOVe files (delete from source after copying).
	RobocopyArgsBuilder& MOV(bool value);

	//MOVE files AND dirs (delete from source after copying).
	RobocopyArgsBuilder& MOVE(bool value);

	//add the given Attributes to copied files.
	RobocopyArgsBuilder& A_plus(std::string_view value);

	//remove the given Attributes from copied files.
	RobocopyArgsBuilder& A_minus(std::string_view value);

	//CREATE directory tree and zero-length files only.
	RobocopyArgsBuilder& CREATE(bool value);

	//create destination files using 8.3 FAT file names only.
	RobocopyArgsBuilder& FAT(bool value);

	//turn off very long path (> 256 characters) support.
	RobocopyArgsBuilder& _256(bool value);

	//MONitor source; run again when more than n changes seen.
	RobocopyArgsBuilder& MON(int value);

	//MOnitor source; run again in m minutes Time, if changed.
	RobocopyArgsBuilder& MOT(int value);

	//Run Hours - times when new copies may be started.
	RobocopyArgsBuilder& RH(int hh1, int mm1, int hh2, int mm2);

	//check run hours on a Per File (not per pass) basis.
	RobocopyArgsBuilder& PF(bool value);

	//Inter-Packet Gap (ms), to free bandwidth on slow lines.
	RobocopyArgsBuilder& IPG(int ms);

	//copy symbolic links versus the target.
	RobocopyArgsBuilder& SL(bool value);

	/*
		Do multi-threaded copies with n threads (default 8).
                       n must be at least 1 and not greater than 128.
                       This option is incompatible with the /IPG and /EFSRAW options.
                       Redirect output using /LOG option for better performance.
	*/
	RobocopyArgsBuilder& MT(int n);

	//what to COPY for directories (default is /DCOPY:DA). (copyflags : D = Data, A = Attributes, T = Timestamps).
	RobocopyArgsBuilder& DCOPY(StringView copyFlags);

	//COPY NO directory info (by default /DCOPY:DA is done).
	RobocopyArgsBuilder& NODCOPY(bool value);

	//copy files without using the Windows Copy Offload mechanism
	RobocopyArgsBuilder& NOOFFLOAD(bool value);
	
#pragma region FileSelectionOptions
	//copy only files with the Archive attribute set.
	RobocopyArgsBuilder& A(bool value);

	//copy only files with the Archive attribute and reset it.
	RobocopyArgsBuilder& M(bool value);

	//Include only files with any of the given Attributes set.
	RobocopyArgsBuilder& IA(StringView value);

	//eXclude files with any of the given Attributes set.
	RobocopyArgsBuilder& XA(StringView value);

	//eXclude Files matching given names/paths/wildcards.
	RobocopyArgsBuilder& XF(std::span<StringView> file);

	//eXclude Directories matching given names/paths.
	RobocopyArgsBuilder& XD(std::span<StringView> dirs);

	//eXclude Changed files.
	RobocopyArgsBuilder& XC(bool value);

	//eXclude Newer files.
	RobocopyArgsBuilder& XN(bool value);

	//eXclude Older files.
	RobocopyArgsBuilder& XO(bool value);

	//eXclude eXtra files and directories.
	RobocopyArgsBuilder& XX(bool value);

	//eXclude Lonely files and directories.
	RobocopyArgsBuilder& XL(bool value);

	//Include Same files.
	RobocopyArgsBuilder& IS(bool value);

	//Include Tweaked files.
	RobocopyArgsBuilder& IT(bool value);

	//MAXimum file size - exclude files bigger than n bytes.
	RobocopyArgsBuilder& MAX(size_t bytes);

	//MINimum file size - exclude files smaller than n bytes.
	RobocopyArgsBuilder& MIN(size_t bytes);

	//MAXimum file AGE - exclude files older than n days/date.
	RobocopyArgsBuilder& MAXAGE(size_t days);

	//MINimum file AGE - exclude files newer than n days/date.
	RobocopyArgsBuilder& MINAGE(size_t days);

	//MAXimum Last Access Date - exclude files unused since n.
	RobocopyArgsBuilder& MAXLAD(size_t days);

	//MINimum Last Access Date - exclude files used since n. (If n < 1900 then n = n days, else n = YYYYMMDD date).
	RobocopyArgsBuilder& MINLAD(size_t days);

	//eXclude Junction points and symbolic links. (normally included by default).
	RobocopyArgsBuilder& XJ(bool value);

	//assume FAT File Times (2-second granularity).
	RobocopyArgsBuilder& FFT(bool value);

	//compensate for one-hour DST time differences.
	RobocopyArgsBuilder& DST(bool value);

	//eXclude Junction points and symbolic links for Directories.
	RobocopyArgsBuilder& XJD(bool value);

	//eXclude symbolic links for Files.
	RobocopyArgsBuilder& XJF(bool value);
#pragma endregion

#pragma region RetryOptions
	//R:n :: number of Retries on failed copies: default 1 million.
	RobocopyArgsBuilder& R(size_t n);
	
	//Wait time between retries: default is 30 seconds.
	RobocopyArgsBuilder& W(size_t seconds);

	//Save /R:n and /W:n in the Registry as default settings.
	RobocopyArgsBuilder& REG(bool value);

	//wait for sharenames To Be Defined (retry error 67).
	RobocopyArgsBuilder& TBD(bool value);
#pragma endregion

#pragma region LoggingOptions

	//List only - don't copy, timestamp or delete any files.
	RobocopyArgsBuilder& L(bool value);

	//report all eXtra files, not just those selected.
	RobocopyArgsBuilder& X(bool value);

	//produce Verbose output, showing skipped files.
	RobocopyArgsBuilder& V(bool value);

	//include source file Time Stamps in the output.
	RobocopyArgsBuilder& TS(bool value);

	//include Full Pathname of files in the output.
	RobocopyArgsBuilder& FP(bool value);

	//Print sizes as bytes.
	RobocopyArgsBuilder& BYTES(bool value);

	//No Size - don't log file sizes.
	RobocopyArgsBuilder& NS(bool value);

	//No Class - don't log file classes.
	RobocopyArgsBuilder& NC(bool value);

	//No File List - don't log file names.
	RobocopyArgsBuilder& NFL(bool value);

	//No Directory List - don't log directory names.
	RobocopyArgsBuilder& NDL(bool value);

	//No Progress - don't display percentage copied.
	RobocopyArgsBuilder& NP(bool value);

	//show Estimated Time of Arrival of copied files.
	RobocopyArgsBuilder& ETA(bool value);

	//output status to LOG file (overwrite existing log).
	RobocopyArgsBuilder& LOG(StringView file);

	//output status to LOG file (append to existing log).
	RobocopyArgsBuilder& LOG_plus(StringView file);

	//output status to LOG file as UNICODE (overwrite existing log).
	RobocopyArgsBuilder& UNILOG(StringView file);

	//output status to LOG file as UNICODE (append to existing log).
	RobocopyArgsBuilder& UNILOG_Plus(StringView file);

	//output to console window, as well as the log file.
	RobocopyArgsBuilder& TEE(bool value);

	//No Job Header.
	RobocopyArgsBuilder& NJH(bool value);

	//No Job Summary.
	RobocopyArgsBuilder& NJS(bool value);

	//output status as UNICODE.
#ifndef UNICODE
	RobocopyArgsBuilder& UNICODE(bool value);
#endif
#pragma endregion
	
#pragma region JobOptions

	//take parameters from the named JOB file.
	RobocopyArgsBuilder& JOB(StringView jobName);

	//SAVE parameters to the named job file
	RobocopyArgsBuilder& SAVE(StringView jobName);

	//QUIT after processing command line (to view parameters).
	RobocopyArgsBuilder& QUIT(bool value);

	//NO Source Directory is specified.
	RobocopyArgsBuilder& NOSD(bool value);

	//NO Destination Directory is specified
	RobocopyArgsBuilder& NODD(bool value);

	//Include the following Files.
	RobocopyArgsBuilder& IF(bool value);
#pragma endregion
};
