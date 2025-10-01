#pragma once
#include "ShellItem.h"
#include <ShlObj_core.h>
class BasicInteraction
{
	ShellItem source;
	ShellItem destination;

	void generateFiles();
	void runImplementation();
public:
	static void Run();
};
