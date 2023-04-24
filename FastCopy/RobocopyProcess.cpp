#include "pch.h"
#include "RobocopyProcess.h"


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
	result += multiThread;
	return result;
}


RobocopyProcess::RobocopyProcess(RobocopyArgs const& arg) : 
	Process{LR"(C:\Windows\System32\robocopy.exe)", arg.ToString()}
{
}
