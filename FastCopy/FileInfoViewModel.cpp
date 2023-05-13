#include "pch.h"
#include "FileInfoViewModel.h"
#if __has_include("FileInfoViewModel.g.cpp")
#include "FileInfoViewModel.g.cpp"
#endif
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#pragma comment(lib, "Gdi32.lib")
#include <CommCtrl.h>
#include <commoncontrols.h>

#include <shellapi.h>
#include <filesystem>



static HICON GetImage(std::wstring_view path)
{
	// Get the image list index of the icon
	SHFILEINFO sfi;
	if (!SHGetFileInfo(path.data(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX)) return NULL;

	// Get the jumbo image list
	IImageList* piml;
	if (FAILED(SHGetImageList(SHIL_JUMBO, IID_PPV_ARGS(&piml)))) return NULL;

	// Extract an icon
	HICON hico;
	piml->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hico);
	
	// Clean up
	piml->Release();
	
	// Return the icon
	return hico;
}

static auto HIconToWriteableBitmap(HICON hIcon)
{
	ICONINFO iconInfo = { 0 };
	GetIconInfo(hIcon, &iconInfo);

	// Get the screen DC
	HDC dc = GetDC(NULL);

	// Get icon size info
	BITMAP bm = { 0 };
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

	// Set up BITMAPINFO
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Extract the color bitmap
	int nBits = bm.bmWidth * bm.bmHeight;
	int32_t* colorBits = new int32_t[nBits];
	GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

	// Check whether the color bitmap has an alpha channel.
	BOOL hasAlpha = FALSE;
	for (int i = 0; i < nBits; i++) {
		if ((colorBits[i] & 0xff000000) != 0) {
			hasAlpha = TRUE;
			break;
		}
	}

	// If no alpha values available, apply the mask bitmap
	if (!hasAlpha) {
		// Extract the mask bitmap
		int32_t* maskBits = new int32_t[nBits];
		GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
		// Copy the mask alphas into the color bits
		for (int i = 0; i < nBits; i++) {
			if (maskBits[i] == 0) {
				colorBits[i] |= 0xff000000;
			}
		}
		delete[] maskBits;
	}

	// Release DC and GDI bitmaps
	ReleaseDC(NULL, dc);
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	DestroyIcon(hIcon);


	{
		winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap bitmap{ bm.bmWidth, bm.bmHeight };
		//IUnknown* pUnk{ reinterpret_cast<IUnknown*>(bitmap->PixelBuffer) };
		//Microsoft::WRL::ComPtr<IBufferByteAccess> bufferByteAccess;
		//HRESULT hr{ pUnk->QueryInterface(IID_PPV_ARGS(&bufferByteAccess)) };
		auto buffer = bitmap.PixelBuffer();
		auto pBuffer = buffer.data();
		for (int i = 0; i < nBits; i++)
		{
			*pBuffer = colorBits[i] & 0xff; ++pBuffer;
			*pBuffer = (colorBits[i] >> 8) & 0xff; ++pBuffer;
			*pBuffer = (colorBits[i] >> 16) & 0xff; ++pBuffer;
			*pBuffer = (colorBits[i] >> 24) & 0xff;

			if (i != nBits - 1) ++pBuffer;
		}
		return bitmap;
	}
}
namespace winrt::FastCopy::implementation
{
    winrt::hstring FileInfoViewModel::Filename()
    {
		return std::filesystem::path{ m_path.data()}.filename().wstring().data();
    }

    winrt::Windows::Foundation::DateTime FileInfoViewModel::CreationDate()
    {
		return std::chrono::clock_cast<winrt::clock>(std::filesystem::last_write_time(std::filesystem::path{ m_path.data()}));
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
		auto icon = GetImage(m_path.data());
		if (icon)
			return HIconToWriteableBitmap(icon);

		winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage image{ winrt::Windows::Foundation::Uri{L"ms-appx:///Assets/FileDefault.ico" } };
		return image;
	}
}
