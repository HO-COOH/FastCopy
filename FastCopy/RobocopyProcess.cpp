#include "pch.h"
#include "RobocopyProcess.h"
#include "RobocopyArgs.h"


RobocopyProcess::RobocopyProcess(RobocopyArgs const& arg) : 
	Process{LR"(C:\Windows\System32\robocopy.exe)", arg.ToString()}
{
}
