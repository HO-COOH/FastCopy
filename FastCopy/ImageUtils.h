#pragma once
#include <string_view>
#include <Windows.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

/**
 * @brief Get HIcon from file
 */
HICON GetHIconFromFile(std::wstring_view path);


/**
 * @brief Get HBitmap from file
 */
HBITMAP GetHBitmapFromFile(std::wstring_view path);


/**
 * @brief HBITMAP -> winrt::Microsoft::UI::Xaml::Media::Imaging::WritableBitmap
 */
winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap HBitmapToWriteableBitmap(
	HBITMAP hBitmap, 
	HBITMAP hMask = NULL);


/**
 * @brief HICON -> winrt::Microsoft>>UI::Xaml::Meida::Imaging::WritableBitmap
 */
winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap HIconToWriteableBitmap(HICON hIcon);