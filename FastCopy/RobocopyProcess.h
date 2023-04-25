#pragma once
#include "../SpeedTest/Process.h"

struct RobocopyArgs;

class RobocopyProcess : public Process<wchar_t>
{
public:
	RobocopyProcess(RobocopyArgs const& arg);
};

