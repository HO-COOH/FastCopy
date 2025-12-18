#pragma once
#include <string_view>

namespace Utils
{
	class TempFileAutoDeleter
	{
		std::wstring_view m_tempFile;
	public:
		TempFileAutoDeleter(std::wstring_view tempFile, bool create);
		~TempFileAutoDeleter();

		TempFileAutoDeleter(TempFileAutoDeleter const&) = delete;
		TempFileAutoDeleter(TempFileAutoDeleter&&) = delete;
		TempFileAutoDeleter& operator=(TempFileAutoDeleter const&) = delete;
		TempFileAutoDeleter& operator=(TempFileAutoDeleter&&) = delete;
	};
}