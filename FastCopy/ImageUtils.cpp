#include "pch.h"
#include "ImageUtils.h"
#pragma comment(lib, "Gdi32.lib")
#include <CommCtrl.h>
#include <commoncontrols.h>

#include <ShObjIdl_core.h>
#include "PathUtils.h"

HICON GetHIconFromFile(std::wstring_view path)
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

HBITMAP GetHBitmapFromFile(std::wstring_view path)
{
	winrt::com_ptr<IShellItem> item = CreateItemFromParsingName(ToBackslash(path).data());
	HBITMAP bitmap{};
	item.as<IShellItemImageFactory>()->GetImage({ 96, 96 }, SIIGBF_BIGGERSIZEOK, &bitmap);
	return bitmap;
}

winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap HBitmapToWriteableBitmap(HBITMAP hBitmap, HBITMAP hMask)
{
	// Get the screen DC
	HDC dc = GetDC(NULL);

	// Get icon size info
	BITMAP bm = { 0 };
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	if (bm.bmWidth == 0 || bm.bmHeight == 0)
		return nullptr;

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
	auto colorBits = std::make_unique<int32_t[]>(nBits);
	GetDIBits(dc, hBitmap, 0, bm.bmHeight, colorBits.get(), &bmi, DIB_RGB_COLORS);

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
		GetDIBits(dc, hMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
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
	DeleteObject(hBitmap);
	if (hMask != NULL)
		DeleteObject(hMask);


	{
		winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap bitmap{ bm.bmWidth, bm.bmHeight };
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

winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap HIconToWriteableBitmap(HICON hIcon)
{
	ICONINFO iconInfo = { 0 };
	GetIconInfo(hIcon, &iconInfo);
	return HBitmapToWriteableBitmap(iconInfo.hbmColor, iconInfo.hbmMask);
}
