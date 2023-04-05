#pragma once
#include <string>

//Represents a single test operation (a file or a folder)
struct TestOperation
{
	//Source path of file/folder
	std::wstring source;

	//Destination path of file/folder
	std::wstring destination;

	//Operation, default to copy
	enum class Operation
	{
		Copy,
		Move,
		Delete
	} operation = Operation::Copy;
};