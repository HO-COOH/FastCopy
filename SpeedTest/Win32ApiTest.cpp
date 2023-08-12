#include "Win32ApiTest.h"
#include <Windows.h>
#include <shellapi.h>
#include "Env.h"
#include <filesystem>


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
		
		std::string sourceBuffer(MAX_PATH + 1, {});
		sourceBuffer.assign(path.source);
		if(path.isFolder())
			sourceBuffer += R"(\*)";

		char destBuffer[MAX_PATH + 1]{}; //for double termination
		std::ranges::copy(path.destination, destBuffer);

		info.pFrom = sourceBuffer.data();
		info.pTo = destBuffer;
		info.fFlags = FOF_NO_UI;
		if (int const result = SHFileOperationA(&info); result != 0)
		{
			Env::Puts(std::format("Error code: {} in SHFileOperationW", result).data());
			return false;
		}
	}
	return true;
}

std::string Win32ApiTest::GetName() const
{
	return "Win32 SHFileOperation";
}
