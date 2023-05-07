#include "pch.h"
#include "FileCompareViewModel.h"
#if __has_include("FileCompareViewModel.g.cpp")
#include "FileCompareViewModel.g.cpp"
#endif

#include "ViewModelLocator.h"

namespace winrt::FastCopy::implementation
{
	FileCompareViewModel::FileCompareViewModel(winrt::hstring path1, winrt::hstring path2) : m_file1{ path1 }, m_file2{ path2 } 
	{
		m_file1.SelectionChanged([this](auto, bool selected) { setRobocopyViewModelDuplicateSelection(true, selected); });
		m_file2.SelectionChanged([this](auto, bool selected) {setRobocopyViewModelDuplicateSelection(false, selected); });
	}

	void FileCompareViewModel::setRobocopyViewModelDuplicateSelection(bool isFile1, bool isSelected)
	{
		auto robocopyViewModel = ViewModelLocator::GetInstance().RobocopyViewModel();
		if (isFile1)
		{
			isSelected ? robocopyViewModel.AddSource() : robocopyViewModel.RemoveSource();
		}
		else
		{
			isSelected ? robocopyViewModel.AddDestination() : robocopyViewModel.RemoveDestination();
		}
	}
}
