#include "Env.h"
#include <ShlObj_core.h>
#include <stdexcept>
namespace Env
{
	static auto getFolderPathImpl(REFKNOWNFOLDERID id)
	{
		wchar_t* ptr;
		SHGetKnownFolderPath(id, 0, NULL, &ptr);
		std::wstring result{ ptr };
		CoTaskMemFree(ptr);
		return result;
	}

	std::wstring const& GetFolderPath(SpecialFolder folder)
	{
		switch (folder)
		{
			case Env::SpecialFolder::Desktop:
			{
				static std::wstring result = getFolderPathImpl(FOLDERID_Desktop);
				return result;
			}
			default:
				throw std::runtime_error{ "Unknwon" };
		}
	}
}
