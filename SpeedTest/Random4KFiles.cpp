#include "Random4KFiles.h"
#include <fstream>

void Random4KFiles::Generate()
{
	auto buffer = makeFilledBuffer();

	for (uint64_t currentBytes{}; currentBytes < m_totalBytes; currentBytes += std::size(buffer))
	{ 
		std::ofstream{ m_source / std::to_string(currentBytes), std::ios::binary }.write(buffer.data(), std::size(buffer));
	}
}
