#pragma once
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/string.hpp>

struct Path
{
    boost::interprocess::basic_string<wchar_t> content;
    
    enum class Type
    {
        Folder,
        File
    } type;
};
