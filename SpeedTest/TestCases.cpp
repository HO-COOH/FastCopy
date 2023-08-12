#include "TestCases.h"
#include "Config.h"
#include <chrono>
#include <ranges>
#include <fstream>
#include <filesystem>
#include <span>
#include <array>
#include "Env.h"

template<typename Char>
static void createFile(std::basic_string_view<Char> path, std::span<char const> buffer, int repeat = 1)
{
	std::ofstream fs{ path.data() , std::ios::binary };
	while(repeat--)
		fs.write(buffer.data(), buffer.size());
}

void fillBuffer(auto& buffer)
{
	std::ranges::generate(buffer, [] { return rand() % UCHAR_MAX; });
}

template<size_t Size = 1024 * 4>
auto makeFilledBuffer()
{
	std::array<char, Size> buffer;
	fillBuffer(buffer);
	return buffer;
}

Random4KFiles::Random4KFiles(uint64_t totalBytes) : m_totalBytes{totalBytes}
{
}

void Random4KFiles::beforeRun()
{
	auto const& config = Config::GetInstance();
	auto const before = std::chrono::steady_clock::now();
	size_t totalBytes{};
	auto buffer = makeFilledBuffer<RandomFileSize>();
	config.CreateSourceAndDestinationFolder();
	while (totalBytes < m_totalBytes) //1GB of 4K random files
	{
		createFile<char>(config.sourceFolder + "\\" + std::to_string(totalBytes), buffer);
		totalBytes += std::size(buffer);
	}
	test.push_back(
		TestOperation{
			config.sourceFolder,
			config.destinationFolder
		}
	);
	Env::Puts(std::format(
		"Make test files ==> {} seconds\n",
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - before)
	).data());
}

BigFile::BigFile(uint64_t bigFileSize) : m_bigFileSize{bigFileSize}
{
}

void BigFile::beforeRun()
{
	auto const& config = Config::GetInstance();
	auto const before = std::chrono::steady_clock::now();
	auto buffer = makeFilledBuffer();
	config.CreateSourceAndDestinationFolder();
	createFile<char>(config.sourceFolder + "\\BigFile", buffer, m_bigFileSize / std::size(buffer));
	Env::Puts(std::format(
		"Make big file ==> {} seconds\n",
		std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - before)
	).data());
	test.push_back(
		TestOperation{
			config.sourceFolder,
			config.destinationFolder
		}
	);
}

MoveFileSamePartition::MoveFileSamePartition(uint64_t bigFileSize) : m_bigFileSize{bigFileSize}
{
}

void MoveFileSamePartition::beforeRun()
{
	auto const& config = Config::GetInstance();
	auto buffer = makeFilledBuffer();
	config.CreateSourceAndDestinationFolder();
	createFile<char>(config.sourceFolder + "\\BigFile", buffer, m_bigFileSize / std::size(buffer));
	auto moveDestination = std::filesystem::path{ config.sourceFolder }.parent_path().string() + "\\testMoveFolder";
	std::filesystem::create_directory(moveDestination);
	test.push_back(TestOperation{
		config.sourceFolder,
		moveDestination,
		TestOperation::Operation::Move
	});
}
