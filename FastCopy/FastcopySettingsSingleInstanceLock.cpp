#include "pch.h"
#include "FastcopySettingsSingleInstanceLock.h"

FastcopySettingsSingleInstanceLock::FastcopySettingsSingleInstanceLock(bool owning)
	: MutexWrapper{ L"FastCopySettingsLock", owning }
{
}
