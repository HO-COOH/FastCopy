#include "pch.h"
#include "RenameUtils.TempFileAutoDeleter.h"
#include <wil/resource.h>

namespace Utils
{
	TempFileAutoDeleter::TempFileAutoDeleter(std::wstring_view tempFile, bool create)
	{
		if (create)
		{
			wil::unique_handle
			{ CreateFileW(
				tempFile.data(),
				GENERIC_WRITE,
				0,
				nullptr,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			) };
		}
	}

	TempFileAutoDeleter::~TempFileAutoDeleter()
	{
		DeleteFile(m_tempFile.data());
	}
}