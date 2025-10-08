#pragma once
#include <string>

/*
	eg:
		New File  		     485	CMakeLists.txt	20:58 -> 20:58
*/

struct NewFile
{
	constexpr static auto Prefix = "New File";
	uint64_t bytes;
	std::string name;

	NewFile(std::string_view data);
};
