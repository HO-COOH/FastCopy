#include "pch.h"
#include "WinRTFileInfo.h"

WinRTFileInfo::WinRTFileInfo(std::wstring_view path) noexcept
{
	if (!GetFileAttributesExW(path.data(), GetFileExInfoStandard, &m_data))
		m_data = {};
}

winrt::Windows::Foundation::DateTime WinRTFileInfo::CreateTime() const noexcept
{
	return winrt::clock::from_FILETIME(m_data.ftCreationTime);
}

winrt::Windows::Foundation::DateTime WinRTFileInfo::LastAccessTime() const noexcept
{
	return winrt::clock::from_FILETIME(m_data.ftLastAccessTime);
}

winrt::Windows::Foundation::DateTime WinRTFileInfo::LastWriteTime() const noexcept
{
	return winrt::clock::from_FILETIME(m_data.ftLastWriteTime);
}
