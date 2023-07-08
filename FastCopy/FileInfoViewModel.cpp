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


namespace winrt::FastCopy::implementation
{
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
		if (value != m_selected)
		{
			m_selected = value;
			raisePropertyChange(L"Selected");
			m_selectionChanged(*this, value);
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

	static std::wstring GetShellPropStringFromPath(LPCWSTR pPath, PROPERTYKEY const& key)
	{
		try 
		{
			// Use CComPtr to automatically release the IShellItem2 interface when the function returns
			// or an exception is thrown.
			winrt::com_ptr<IShellItem2> pItem = CreateItemFromParsingName<IShellItem2>(pPath);

			// Use CComHeapPtr to automatically release the string allocated by the shell when the function returns
			// or an exception is thrown (calls CoTaskMemFree).
			CComHeapPtr<WCHAR> pValue;
			winrt::check_hresult(pItem->GetString(key, &pValue));

			// Copy to wstring for convenience
			return std::wstring(pValue);
		}
		catch (...) {}
		return {};
	}

	winrt::Windows::Foundation::Collections::IVector<winrt::Windows::Foundation::IInspectable> FileInfoViewModel::tooltipInfo()
	{
		for (auto&& [name, key] : 
			{
				std::pair{L"FileDescription", PKEY_FileDescription},
				std::pair{L"ProductName", PKEY_Software_ProductName},
				std::pair{L"FileVersion", PKEY_FileVersion},
				std::pair{L"Copyright", PKEY_Copyright } 
			})
		{
			auto value = GetShellPropStringFromPath(m_path.data(), key);
			if (value.empty())
				continue;
			m_tooltipInfo.push_back(winrt::box_value(winrt::FastCopy::ExtendedFileInfo{.name = name, .value = value.data()}));
		}
		return winrt::single_threaded_vector(std::move(m_tooltipInfo));
	}
}
