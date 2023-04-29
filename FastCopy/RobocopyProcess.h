#pragma once
#include "../SpeedTest/Process.h"

//Forward declaration
struct RobocopyArgs;

/**
 * @brief Represents a robocopy process
 */
class RobocopyProcess : public Process<wchar_t>
{
public:
	RobocopyProcess(RobocopyArgs const& arg);
};

/**
 * @brief Represents the status of a invoke
 */
enum class Status
{
	Running,
	Pause,
	Cancel
};
