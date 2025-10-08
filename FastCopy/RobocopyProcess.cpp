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

void RobocopyProcess::WaitForExit()
{
	ios.run();
	m_child.wait();
	std::cout << "Child exit with code: " << m_child.native_exit_code() << '\n';
}
