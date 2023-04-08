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
			case Env::SpecialFolder::System32:
			{
				static std::wstring result = getFolderPathImpl(FOLDERID_System);
				return result;
			}
			case Env::SpecialFolder::Windows:
			{
				static std::wstring result = getFolderPathImpl(FOLDERID_Windows);
				return result;
			}
			default:
				throw std::runtime_error{ "Unknwon" };
		}
	}

	static bool HasEnoughSpace(std::wstring_view drive, size_t bytes)
	{
		ULARGE_INTEGER availableBytes{};
		return GetDiskFreeSpaceExW(
			drive.data(),
			&availableBytes,
			nullptr,
			nullptr
		) && bytes <= availableBytes.QuadPart;
	}

	static std::wstring const& GetAvailableDrive(size_t bytes)
	{
		static std::wstring ret = L"E:";
		return ret;
	}

	std::wstring const& GetTestDestinationPath(std::wstring_view subFolder, size_t bytes)
	{
		static std::wstring ret = std::wstring{ L"E:" } + subFolder.data();
		return ret;
	}

	std::wstring const& GetRandomName()
	{
		static std::wstring ret = L"\\test";
		return ret;
	}
}
