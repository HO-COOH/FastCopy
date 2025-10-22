#include "pch.h"
#include "RobocopyProcess.h"
#include "RobocopyArgs.h"

void RobocopyProcess::runContext()
{
	static bool v = [] {
		std::thread
		{
			[] 
			{
				ios.run();
			}
		}.detach();
		return true;
	}();
}

std::regex& RobocopyProcess::progressRegex()
{
	static std::regex Progress{ "^[0-9]+[.]?[0-9]*%" };
	return Progress;
}

void RobocopyProcess::WaitForExit()
{
	ios.run();
	m_child.wait();
	std::cout << "Child exit with code: " << m_child.native_exit_code() << '\n';
}
