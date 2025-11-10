#include "pch.h"
#include "FileInfoViewModel.h"
#if __has_include("FileInfoViewModel.g.cpp")
#include "FileInfoViewModel.g.cpp"
#endif

#include "ImageUtils.h"
#include <filesystem>
#include <Propkey.h>   // PKEY_* constants
#include <shellapi.h>
#include <atlbase.h>
#include <ShlObj_core.h>
#include <wil/com.h>
#include "ViewModelLocator.h"
#include "RobocopyViewModel.h"
#include "PathUtils.h"

namespace winrt::FastCopy::implementation
{
	FileInfoViewModel::FileInfoViewModel(winrt::hstring path, bool isSource) : WinRTFileTime{ path }, m_path{ ToBackslash(path) }, m_isSource{ isSource }
	{
	}

    winrt::hstring FileInfoViewModel::Filename()
    {
		return std::filesystem::path{ m_path.data()}.filename().wstring().data();
    }

    uint64_t FileInfoViewModel::Bytes()
    {
		return std::filesystem::file_size(std::filesystem::path{ m_path.data() });
    }

    void FileInfoViewModel::Selected(bool value)
    {
		if(m_selected != value)
		{ 
			m_selected = value;
			raisePropertyChange(L"Selected");
			auto robocopyViewModelImpl = winrt::get_self<RobocopyViewModel>(ViewModelLocator::GetInstance().RobocopyViewModel());
			value?
				(
					m_isSource? 
						robocopyViewModelImpl->AddSource() :
						robocopyViewModelImpl->AddDestination()
				) :
				(
					m_isSource?
						robocopyViewModelImpl->RemoveSource() :
						robocopyViewModelImpl->RemoveDestination()
				);
		}
    }

	winrt::Microsoft::UI::Xaml::Media::ImageSource FileInfoViewModel::Bitmap()
	{
		try 
		{
			if (auto icon = GetHBitmapFromFile(m_path.data()))
				return HBitmapToWriteableBitmap(icon);
		}
		catch (...) {}

		try 
		{
			if (auto icon = GetHIconFromFile(m_path.data()))				
				return HIconToWriteableBitmap(icon);
		}
		catch (...) {}

		winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage image{ winrt::Windows::Foundation::Uri{L"ms-appx:///Assets/FileDefault.ico" } };
		return image;
	}

	static auto GetShellPropStringFromPath(LPCWSTR pPath, PROPERTYKEY const& key)
	{
		winrt::com_ptr<IShellItem2> pItem = CreateItemFromParsingName<IShellItem2>(pPath);

		wil::unique_cotaskmem_string pValue;
		pItem->GetString(key, &pValue);
		return pValue;
	}

	winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> FileInfoViewModel::tooltipInfo()
	{
		std::vector<winrt::Windows::Foundation::IInspectable> m_tooltipInfo;
		m_tooltipInfo.reserve(4);
		for (auto&& [name, key] : 
			{
				std::pair{L"FileDescription", PKEY_FileDescription},
				std::pair{L"ProductName", PKEY_Software_ProductName},
				std::pair{L"FileVersion", PKEY_FileVersion},
				std::pair{L"Copyright", PKEY_Copyright } 
			})
		{
			try 
			{
				auto value = GetShellPropStringFromPath(m_path.data(), key);
				if (!value)
					continue;
				m_tooltipInfo.push_back(winrt::box_value(winrt::FastCopy::ExtendedFileInfo{ .name = name, .value = value.get() }));
			}
			catch(...){}
		}
		return winrt::single_threaded_vector(std::move(m_tooltipInfo));
	}
}
