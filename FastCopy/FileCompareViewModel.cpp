#include "pch.h"
#include "FileCompareViewModel.h"
#if __has_include("FileCompareViewModel.g.cpp")
#include "FileCompareViewModel.g.cpp"
#endif

#include "ViewModelLocator.h"

namespace winrt::FastCopy::implementation
{
	FileCompareViewModel::FileCompareViewModel(winrt::hstring path1, winrt::hstring path2) : m_file1{ path1, true }, m_file2{ path2, false } 
	{
	}
}
