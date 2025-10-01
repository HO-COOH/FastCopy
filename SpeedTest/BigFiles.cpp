#include "BigFiles.h"
#include <fstream>

void BigFile::Generate()
{
	auto buffer = makeFilledBuffer();
	for (uint64_t currentBytes{}; currentBytes < m_bigFileSize;)
	{
		constexpr static uint64_t SingleFileSize = 1024 * 1024 * 1024;
		std::ofstream f{ m_source / std::to_string(currentBytes), std::ios::binary };
		for (uint64_t currentFileBytes{}; currentFileBytes < SingleFileSize; currentFileBytes += std::size(buffer))
		{
			f.write(buffer.data(), std::size(buffer));
		}
		currentBytes += SingleFileSize;
	}
}
