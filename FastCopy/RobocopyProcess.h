#pragma once
#include "../SpeedTest/Process.h"

//Forward declaration
struct RobocopyArgs;

/**
 * @brief Represents a robocopy process
 */
class RobocopyProcess : public Process<wchar_t>
{
public:
	RobocopyProcess(RobocopyArgs const& arg);
};

/**
 * @brief Represents the status of a invoke
 */
enum class Status
{
	Running,
	Pause,
	Cancel
};

//https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/robocopy#exit-return-codes
enum class RoboCopyExitCodes
{
	//No files were copied. No failure was encountered. No files were mismatched. The files already exist in the destination directory; therefore, the copy operation was skipped.
	NoCopy = 0,

	//All files were copied successfully.
	Success = 1,

	//There are some additional files in the destination directory that aren't present in the source directory. No files were copied.
	NoCopyExtraFiles = 2,

	//Some files were copied. Additional files were present. No failure was encountered.
	PartialCopyExtraFiles = 3,

	//Some files were copied. Some files were mismatched. No failure was encountered.
	PartialCopyMismatchFiles = 5,

	//Additional files and mismatched files exist. No files were copied and no failures were encountered meaning that the files already exist in the destination directory.
	NoCopyExtraFilesAndMismatchFiles = 6,

	//Files were copied, a file mismatch was present, and additional files were present.
	PartialCopyExtraFilesAndMismatchedFiles = 7,

	//Several files didn't copy.
	PartialError = 8
};