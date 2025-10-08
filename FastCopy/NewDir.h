#pragma once
#include <string>

/*eg:  
	New Dir          2    F : \testSource\subFolder
*/
struct NewDir
{
	constexpr static auto Prefix = "New Dir";

	int count;
	std::string fullPath;

	NewDir(std::string_view data);
};
