#include "pch.h"
#include "FastCopyCommandParser.h"

AnonymousPipe::AnonymousPipe()
{
	SECURITY_ATTRIBUTES saAttr;
	// Set the bInheritHandle flag so pipe handles are inherited. 

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	auto success = CreatePipe(&m_readPipe, &m_writePipe, &saAttr, 0);
	assert(success);

	success = SetHandleInformation(m_readPipe, HANDLE_FLAG_INHERIT, 0);
	assert(success);

}