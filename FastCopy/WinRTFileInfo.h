#pragma once
#include <windows.h>
#include <string_view>
#include <winrt/Windows.Foundation.h>

class WinRTFileInfo
{
	WIN32_FILE_ATTRIBUTE_DATA m_data;
public:
	WinRTFileInfo(std::wstring_view path) noexcept;

	winrt::Windows::Foundation::DateTime CreateTime() const noexcept;
	winrt::Windows::Foundation::DateTime LastAccessTime() const noexcept;
	winrt::Windows::Foundation::DateTime LastWriteTime() const noexcept;
	constexpr uint64_t Bytes() const noexcept
	{
		return (static_cast<uint64_t>(m_data.nFileSizeHigh) << 32) | m_data.nFileSizeLow;
	}
};
