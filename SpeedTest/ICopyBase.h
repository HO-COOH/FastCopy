#pragma once

#include <vector>
#include <string>
#include "TestOperation.h"
#include <functional>

//All test inherits this class
class IImplementationBase
{
public:
	/**
	 * The methods to be called to run the copy/move program or apis 
	 * 
	 * @param paths All the files to be copied or moved
	 * @return Whether the call is success
	 */
	virtual bool Run(std::vector<TestOperation>const& paths) = 0;

	/**
	 * Return the name/id of this implementation
	 */
	virtual std::string GetName() const = 0;

	std::function<void(void)> started;
	std::function<void(void)> finished;
};