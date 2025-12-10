#include "pch.h"
#include "Ntdll.h"

NtDll::NtSuspendProcessFunction NtDll::s_ntSuspendFunctionPointer()
{
	static auto pointer = reinterpret_cast<NtSuspendProcessFunction>(
		GetProcAddress
		(
			GetModuleHandleW(L"ntdll.dll"),
			"NtSuspendProcess"
		)
	);
	return pointer;
}

NtDll::NtResumeProcessFunction NtDll::s_ntResumeFunctionPointer()
{
	static auto pointer = reinterpret_cast<NtResumeProcessFunction>(
		GetProcAddress
		(
			GetModuleHandleW(L"ntdll.dll"),
			"NtResumeProcess"
		)
	);
	return pointer;
}

LONG NtDll::NtSuspendProcess(HANDLE processHandle)
{
	return s_ntSuspendFunctionPointer()(processHandle);
}

LONG NtDll::NtResumeProcess(HANDLE processHandle)
{
	return s_ntResumeFunctionPointer()(processHandle);
}
