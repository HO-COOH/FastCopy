#pragma once
#include "ShellItem.h"
#include <ShlObj_core.h>
#include <memory>

class TestCaseBase;

class BasicInteraction
{
	/*ShellItem source;
	ShellItem destination;*/

	std::unique_ptr<TestCaseBase> test;
	bool m_forceSleep{};
	void generateFiles();
	void runImplementation();
public:
	static void Run();
};
