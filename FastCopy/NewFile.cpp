#include "pch.h"
#include "NewFile.h"

void NewFile::Clear() noexcept
{
	bytes = {};
	name.clear();
}
