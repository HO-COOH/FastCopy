#pragma once
#include <wil/com.h>
#include <shlobj_core.h>

class KnwonFolderPath
{
	wil::unique_cotaskmem_string m_path;
public:
	KnwonFolderPath(REFKNOWNFOLDERID id, DWORD flags = 0);

	PWSTR Get() const noexcept;
};
