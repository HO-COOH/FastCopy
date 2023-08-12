#include "Env.h"
#include <ShlObj_core.h>
#include <stdexcept>
#include <format>
#include <chrono>
#include "Config.h"
#include <algorithm>
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
		static std::wstring ret = [bytes]
		{
			for (wchar_t drive = 'D'; drive <= 'Z'; ++drive)
			{
				auto driveStr = std::wstring{ drive } + L":";
				if (HasEnoughSpace(driveStr, bytes))
				{
					return driveStr + L"\\";
				}
			}
			throw std::runtime_error{ "No available drive for test" };
		}();
		return ret;
	}

	std::wstring const& GetTestDestinationPath(std::wstring_view subFolder, size_t bytes)
	{
		static std::wstring ret = GetAvailableDrive(bytes) + subFolder.data();
		return ret;
	}

	std::wstring const& GetRandomName()
	{
		static std::wstring ret = [] 
		{
			auto ret = std::format(L"{}", std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() }.get_local_time());
			std::ranges::replace_if(ret, [](auto c) {return c == L'.' || c == L':'; }, L'-');
			return ret;
		}();
		return ret;
	}

	void Puts(char const* const arg)
	{
		if (!Config::GetInstance().m_printConsole)
			return;

		std::puts(arg);
	}
}
