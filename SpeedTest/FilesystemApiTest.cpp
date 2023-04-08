#include "FilesystemApiTest.h"
#include <filesystem>
#include <execution>
bool FilesystemApiTest::Run(std::vector<TestOperation> const& paths)
{
	try
	{
		std::for_each(
			std::execution::par_unseq,
			std::begin(paths),
			std::end(paths),
			[](TestOperation const& test) {
				std::filesystem::copy(test.source, test.destination, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
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
