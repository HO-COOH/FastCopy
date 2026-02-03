#include "pch.h"
#include "JobObject.h"

JobObject::JobObject()
{
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION limitInfo
	{
		.BasicLimitInformation =
		{
			.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
		}
	};
	winrt::check_bool(SetInformationJobObject(
		m_jobObject.get(),
		JobObjectExtendedLimitInformation,
		&limitInfo,
		sizeof(limitInfo)
	));
}

JobObject& JobObject::operator<<(HANDLE processHandle)
{
	winrt::check_bool(AssignProcessToJobObject(m_jobObject.get(), processHandle));
	return *this;
}


