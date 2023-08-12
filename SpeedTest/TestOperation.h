#pragma once
#include <string>
#include <filesystem>

//Represents a single test operation (a file or a folder)
struct TestOperation
{
	//Source path of file/folder
	std::string source;

	//Destination path of file/folder
	std::string destination;

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