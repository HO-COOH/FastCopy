#include "Win32ApiTest.h"
#include <Windows.h>
#include <shellapi.h>
#include <iostream>

//static auto StringToMultiString(std::wstring_view src)
//{
//	auto buffer = std::make_unique<wchar_t[]>(src.size() + 2);
//	std::copy(src.begin(), src.end(), buffer.get());
//	buffer[src.size()] = L'\0';
//	buffer[src.size() + 1] = L'\0';
//	return buffer;
//}

bool Win32ApiTest::Run(std::vector<TestOperation> const& paths)
{
	for (auto const& path : paths)
	{
		SHFILEOPSTRUCTA info{};
		switch (path.operation)
		{
			case TestOperation::Operation::Copy:	info.wFunc = FO_COPY;	break;
			case TestOperation::Operation::Move:	info.wFunc = FO_MOVE;	break;
			case TestOperation::Operation::Delete:	info.wFunc = FO_DELETE; break;
			default:
				throw std::runtime_error{ "Unknown operation" };
		}
		info.pFrom = path.source.data();
		info.pTo = path.destination.data();
		info.fFlags = FOF_NO_UI;
		if (int const result = SHFileOperationA(&info); result != 0)
		{
			std::cerr << "Error code: " << result << " in SHFileOperationW\n";
			return false;
		}
	}
	return true;
}

std::string Win32ApiTest::GetName() const
{
	return "Win32 SHFileOperation";
}
