#pragma once
#include <string>
#include <filesystem>

//Represents a single test operation (a file or a folder)
template<typename Char>
struct BasicTestOperation
{
	//Source path of file/folder
	std::basic_string<Char> source;

	//Destination path of file/folder
	std::basic_string<Char> destination;

	//Operation, default to copy
	enum class Operation
	{
		Copy,
		Move,
		Delete
	} operation = Operation::Copy;

	bool isFolder() const
	{
		return std::filesystem::is_directory(source);
	}
};

using TestOperation = BasicTestOperation<wchar_t>;
