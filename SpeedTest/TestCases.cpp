#include "TestCases.h"
#include "Config.h"
#include <chrono>
#include <ranges>
#include <fstream>
#include <filesystem>
static void createFile(std::wstring_view path, std::span<char const> buffer)
{
	std::ofstream fs{ path.data() , std::ios::binary };
	fs.write(buffer.data(), buffer.size());
}

void Random4KFiles::operator()(std::vector<TestOperation>& test) const
{
	auto const& config = Config::GetInstance();
	auto const before = std::chrono::steady_clock::now();
	size_t totalBytes{};
	char buffer[1024 * 4]{};
	std::ranges::generate(buffer, [] {return rand() % 255; });
	std::filesystem::create_directory(config.sourceFolder);
	while (totalBytes < config.randomFileTotalBytes) //1GB of 4K random files
	{
		createFile(config.sourceFolder + L"\\" + std::to_wstring(totalBytes), buffer);
		totalBytes += std::size(buffer);
	}
	test.push_back(
		TestOperation{
			config.sourceFolder,
			config.destinationFolder
		}
	);
	std::filesystem::create_directory(config.destinationFolder);
	puts(std::format(
		"Make test files ==> {} seconds\n",
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - before)
	).data());
}
