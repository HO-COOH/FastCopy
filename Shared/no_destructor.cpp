//#include "pch.h"
#include <atomic>
#include "no_destructor.h"

// static
std::atomic<bool> NoDestructorIfTerminatingBase::m_is_terminating = false;