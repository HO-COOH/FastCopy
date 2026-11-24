#include "pch.h"
#include "FileCompareViewModel.h"
#if __has_include("FileCompareViewModel.g.cpp")
#include "FileCompareViewModel.g.cpp"
#endif
#include "FileInfoViewModel.h"

namespace winrt::FastCopy::implementation
{
	FileCompareViewModel::FileCompareViewModel(
		std::wstring_view path1,
		std::wstring_view path2,
		RobocopyViewModel& robocopyViewModel
	) :
		m_file1{ winrt::make_self<FileInfoViewModel>(path1, true, robocopyViewModel) },
		m_file2{ winrt::make_self<FileInfoViewModel>(path2, false, robocopyViewModel) }
	{
	}

	FileCompareViewModel::~FileCompareViewModel() = default;

	winrt::FastCopy::FileInfoViewModel FileCompareViewModel::File1()
	{
		return *m_file1;
	}

	winrt::FastCopy::FileInfoViewModel FileCompareViewModel::File2()
	{
		return *m_file2;
	}
}
