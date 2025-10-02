#include "Random4KFiles.h"
#include <fstream>

void Random4KFiles::Generate()
{
	auto buffer = makeFilledBuffer();

	std::filesystem::create_directories(m_source);
	for (uint64_t currentBytes{}; currentBytes < m_totalBytes; currentBytes += std::size(buffer))
	{ 
		std::ofstream fs{ m_source / std::to_string(currentBytes), std::ios::binary };
		if(!fs.is_open())
			throw std::runtime_error{ "Cannot open file for writing" };
		fs.write(buffer.data(), std::size(buffer));
	}
}
