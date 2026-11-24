#pragma once
#include <string>
#include <string_view>
#include <span>

class RobocopyArgsBuilder
{
	std::wstring m_arg = LR"("{}" "{}" )";
	bool m_sourceSet = false;
	bool m_destinationSet = false;
public:
	constexpr static int MT_Min = 1;
	constexpr static int MT_Max = 128;

	RobocopyArgsBuilder() = default;

	std::wstring const& Build() const;

	//Source Directory (drive:\path or \\server\share\path)
	RobocopyArgsBuilder& Source(std::wstring_view value);

	//Destination Dir  (drive:\path or \\server\share\path).
	RobocopyArgsBuilder& Destination(std::wstring_view value);

	//File(s) to copy  (names/wildcards: default is "*.*").
	RobocopyArgsBuilder& File(std::span<std::wstring_view> value);

#pragma region CopyOptions
	//copy Subdirectories, but not empty ones.
	RobocopyArgsBuilder& S();

	//copy subdirectories, including Empty ones.
	RobocopyArgsBuilder& E();

	//only copy the top n LEVels of the source directory tree
	RobocopyArgsBuilder& LEV(int n);

	//copy files in restartable mode.
	RobocopyArgsBuilder& Z();

	//copy files in Backup mode.
	RobocopyArgsBuilder& B();

	//use restartable mode; if access denied use Backup mode.
	RobocopyArgsBuilder& ZB();

	//copy using unbuffered I/O (recommended for large files).
	RobocopyArgsBuilder& J();

	//copy all encrypted files in EFS RAW mode.
	RobocopyArgsBuilder& EFSRAW();

	/*
	* what to COPY for files (default is /COPY:DAT).
                       (copyflags : D=Data, A=Attributes, T=Timestamps).
                       (S=Security=NTFS ACLs, O=Owner info, U=aUditing info).
	*/
	RobocopyArgsBuilder& COPY(std::wstring_view copyflags);

	//copy files with SECurity (equivalent to /COPY:DATS).
	RobocopyArgsBuilder& SEC();

	//COPY ALL file info (equivalent to /COPY:DATSOU).
	RobocopyArgsBuilder& COPYALL();

	//COPY NO file info (useful with /PURGE).
	RobocopyArgsBuilder& NOCOPY();

	//FIX file SECurity on all files, even skipped files.
	RobocopyArgsBuilder& SECFIX();

	//FIX file TIMes on all files, even skipped files.
	RobocopyArgsBuilder& TIMFIX();

	//delete dest files/dirs that no longer exist in source.
	RobocopyArgsBuilder& PURGE();

	//MIRror a directory tree (equivalent to /E plus /PURGE).
	RobocopyArgsBuilder& MIR();

	//MOVe files (delete from source after copying).
	RobocopyArgsBuilder& MOV();

	//MOVE files AND dirs (delete from source after copying).
	RobocopyArgsBuilder& MOVE();

	//add the given Attributes to copied files.
	RobocopyArgsBuilder& A_plus(std::wstring_view value);

	//remove the given Attributes from copied files.
	RobocopyArgsBuilder& A_minus(std::wstring_view value);

	//CREATE directory tree and zero-length files only.
	RobocopyArgsBuilder& CREATE();

	//create destination files using 8.3 FAT file names only.
	RobocopyArgsBuilder& FAT();

	//turn off very long path (> 256 characters) support.
	RobocopyArgsBuilder& _256();

	//MONitor source; run again when more than n changes seen.
	RobocopyArgsBuilder& MON(int value);

	//MOnitor source; run again in m minutes Time, if changed.
	RobocopyArgsBuilder& MOT(int value);

	//Run Hours - times when new copies may be started.
	RobocopyArgsBuilder& RH(int hh1, int mm1, int hh2, int mm2);

	//check run hours on a Per File (not per pass) basis.
	RobocopyArgsBuilder& PF();

	//Inter-Packet Gap (ms), to free bandwidth on slow lines.
	RobocopyArgsBuilder& IPG(int ms);

	//copy symbolic links versus the target.
	RobocopyArgsBuilder& SL();

	/*
		Do multi-threaded copies with n threads (default 8).
                       n must be at least 1 and not greater than 128.
                       This option is incompatible with the /IPG and /EFSRAW options.
                       Redirect output using /LOG option for better performance.
	*/
	RobocopyArgsBuilder& MT(int n);

	//what to COPY for directories (default is /DCOPY:DA). (copyflags : D = Data, A = Attributes, T = Timestamps).
	RobocopyArgsBuilder& DCOPY(std::wstring_view copyFlags);

	//COPY NO directory info (by default /DCOPY:DA is done).
	RobocopyArgsBuilder& NODCOPY();

	//copy files without using the Windows Copy Offload mechanism
	RobocopyArgsBuilder& NOOFFLOAD();
#pragma endregion

#pragma region FileSelectionOptions
	//copy only files with the Archive attribute set.
	RobocopyArgsBuilder& A();

	//copy only files with the Archive attribute and reset it.
	RobocopyArgsBuilder& M();

	//Include only files with any of the given Attributes set.
	RobocopyArgsBuilder& IA(std::wstring_view value);

	//eXclude files with any of the given Attributes set.
	RobocopyArgsBuilder& XA(std::wstring_view value);

	//eXclude Files matching given names/paths/wildcards.
	RobocopyArgsBuilder& XF(std::span<std::wstring_view> file);

	//eXclude Directories matching given names/paths.
	RobocopyArgsBuilder& XD(std::span<std::wstring_view> dirs);

	//eXclude Changed files.
	RobocopyArgsBuilder& XC();

	//eXclude Newer files.
	RobocopyArgsBuilder& XN();

	//eXclude Older files.
	RobocopyArgsBuilder& XO();

	//eXclude eXtra files and directories.
	RobocopyArgsBuilder& XX();

	//eXclude Lonely files and directories.
	RobocopyArgsBuilder& XL();

	//Include Same files.
	RobocopyArgsBuilder& IS();

	//Include Tweaked files.
	RobocopyArgsBuilder& IT();

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
	RobocopyArgsBuilder& XJ();

	//assume FAT File Times (2-second granularity).
	RobocopyArgsBuilder& FFT();

	//compensate for one-hour DST time differences.
	RobocopyArgsBuilder& DST();

	//eXclude Junction points and symbolic links for Directories.
	RobocopyArgsBuilder& XJD();

	//eXclude symbolic links for Files.
	RobocopyArgsBuilder& XJF();
#pragma endregion

#pragma region RetryOptions
	//R:n :: number of Retries on failed copies: default 1 million.
	RobocopyArgsBuilder& R(size_t n);
	
	//Wait time between retries: default is 30 seconds.
	RobocopyArgsBuilder& W(size_t seconds);

	//Save /R:n and /W:n in the Registry as default settings.
	RobocopyArgsBuilder& REG();

	//wait for sharenames To Be Defined (retry error 67).
	RobocopyArgsBuilder& TBD();
#pragma endregion

#pragma region LoggingOptions

	//List only - don't copy, timestamp or delete any files.
	RobocopyArgsBuilder& L();

	//report all eXtra files, not just those selected.
	RobocopyArgsBuilder& X();

	//produce Verbose output, showing skipped files.
	RobocopyArgsBuilder& V();

	//include source file Time Stamps in the output.
	RobocopyArgsBuilder& TS();

	//include Full Pathname of files in the output.
	RobocopyArgsBuilder& FP();

	//Print sizes as bytes.
	RobocopyArgsBuilder& BYTES();

	//No Size - don't log file sizes.
	RobocopyArgsBuilder& NS();

	//No Class - don't log file classes.
	RobocopyArgsBuilder& NC();

	//No File List - don't log file names.
	RobocopyArgsBuilder& NFL();

	//No Directory List - don't log directory names.
	RobocopyArgsBuilder& NDL();

	//No Progress - don't display percentage copied.
	RobocopyArgsBuilder& NP();

	//show Estimated Time of Arrival of copied files.
	RobocopyArgsBuilder& ETA();

	//output status to LOG file (overwrite existing log).
	RobocopyArgsBuilder& LOG(std::wstring_view file);

	//output status to LOG file (append to existing log).
	RobocopyArgsBuilder& LOG_plus(std::wstring_view file);

	//output status to LOG file as UNICODE (overwrite existing log).
	RobocopyArgsBuilder& UNILOG(std::wstring_view file);

	//output status to LOG file as UNICODE (append to existing log).
	RobocopyArgsBuilder& UNILOG_Plus(std::wstring_view file);

	//output to console window, as well as the log file.
	RobocopyArgsBuilder& TEE();

	//No Job Header.
	RobocopyArgsBuilder& NJH();

	//No Job Summary.
	RobocopyArgsBuilder& NJS();

	//output status as UNICODE.
	RobocopyArgsBuilder& Unicode();
#pragma endregion
	
#pragma region JobOptions

	//take parameters from the named JOB file.
	RobocopyArgsBuilder& JOB(std::wstring_view jobName);

	//SAVE parameters to the named job file
	RobocopyArgsBuilder& SAVE(std::wstring_view jobName);

	//QUIT after processing command line (to view parameters).
	RobocopyArgsBuilder& QUIT();

	//NO Source Directory is specified.
	RobocopyArgsBuilder& NOSD();

	//NO Destination Directory is specified
	RobocopyArgsBuilder& NODD();

	//Include the following Files.
	RobocopyArgsBuilder& IF();
#pragma endregion
};
