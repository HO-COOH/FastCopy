#pragma once
#include "TestFactory.h"

template<typename Self>
class AutoRegisterImpl
{
public:
	AutoRegisterImpl()
	{
		TestFactory::Register(std::make_unique<Self>());
	}
};

/**
 * This class auto register the copy implementation, 
 * simply inheriting from this class, and it will be added to the factory
 */
template<typename Self>
class AutoRegister
{
	static inline AutoRegisterImpl<Self> m_impl;
};