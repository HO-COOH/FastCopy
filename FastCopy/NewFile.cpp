#include "pch.h"
#include "NewFile.h"

void NewFile::Clear() noexcept
{
	bytes = {};
	name.clear();
}

NewFile::operator bool() const
{
	return !name.empty();
}
