#pragma once
#include <Windows.h>

class NtDll
{
	using NtSuspendProcessFunction = LONG(NTAPI*)(HANDLE processHandle);
	using NtResumeProcessFunction = LONG(NTAPI*)(HANDLE processHandle);

	static NtSuspendProcessFunction s_ntSuspendFunctionPointer();
	static NtResumeProcessFunction s_ntResumeFunctionPointer();
public:
	static LONG NtSuspendProcess(HANDLE processHandle);
	static LONG NtResumeProcess(HANDLE processHandle);
};
