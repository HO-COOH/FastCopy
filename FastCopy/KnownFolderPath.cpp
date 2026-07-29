#include "pch.h"
#include "KnownFolderPath.h"

KnwonFolderPath::KnwonFolderPath(REFKNOWNFOLDERID id, DWORD flags)
{
	winrt::check_hresult(SHGetKnownFolderPath(
		id,
		flags,
		nullptr,
		m_path.put()
	));
}

PWSTR KnwonFolderPath::Get() const noexcept
{
	return m_path.get();
}
