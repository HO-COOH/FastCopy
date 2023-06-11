#include "FilesystemApiTest.h"
#include <filesystem>
#include <execution>
#include <cassert>
bool FilesystemApiTest::Run(std::vector<TestOperation> const& paths)
{
	try
	{
		std::for_each(
			std::execution::par_unseq,
			std::begin(paths),
			std::end(paths),
			[](TestOperation const& test) {
				switch (test.operation)
				{
					case TestOperation::Operation::Copy:
						std::filesystem::copy(test.source, test.destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
						break;
					case TestOperation::Operation::Move:
						std::filesystem::rename(test.source, test.destination);
						break;
					default:
						assert(false);
				}
			}
		);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

std::string FilesystemApiTest::GetName() const
{
	return "C++ std::filesystem";
}
