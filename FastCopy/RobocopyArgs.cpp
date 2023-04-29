#include "pch.h"
#include "RobocopyArgs.h"

std::wstring RobocopyArgs::ToString() const
{
	auto result =
		std::format(LR"(C:\Windows\System32\robocopy.exe "{}" "{}" )",
			sourceDir,
			destinationDir
		);
	for (auto file : files)
	{
		result += std::format(LR"("{}" )", file);
	}
	result += copySubDirectoriesExceptEmpty;
	result += copySubDirectoriesIncludeEmpty;
	result += moveFiles;
	result += moveFilesAndDirs;
	result += unbufferedIO;
	result += multiThread;
	return result;
}
