#pragma once
#include "MutexWrapper.h"

class FastcopySettingsSingleInstanceLock : public MutexWrapper
{
public:
	FastcopySettingsSingleInstanceLock(bool owning = true);
};
