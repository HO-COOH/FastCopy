#pragma once
#include <wil/resource.h>
#include "jobapi2.h"

class JobObject
{
	wil::unique_handle m_jobObject{ CreateJobObject(nullptr, nullptr) };
public:
	JobObject();

	JobObject& operator<<(HANDLE processHandle);
};