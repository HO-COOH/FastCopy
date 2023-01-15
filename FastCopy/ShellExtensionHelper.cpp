#include "pch.h"
#include "ShellExtensionHelper.h"

ShellExtensionHelper::ShellExtensionHelper(std::string_view name) : sharedMem{ boost::interprocess::open_only, name.data()}
{
}

ShellExtensionHelper::MyVector& ShellExtensionHelper::get(std::string_view name)
{
    auto myVector = sharedMem.find<MyVector>(name.data()).first;
    //MessageBox(NULL, std::to_wstring(myVector->size()).data(), L"", 0);
    //MessageBox(NULL, (*myVector)[0].content, L"", 0);
    return *myVector;
}
