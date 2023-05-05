#pragma once
#include <fstream>
#include "CopyOperation.h"
class ShellItem;

class Recorder
{
public:
	Recorder(CopyOperation op);
	Recorder& operator<<(ShellItem& item);
	~Recorder();
	static bool HasRecord();
private:
	FILE* m_fs;

	/**
	 * Return the file name of the record file
	 * @example "C:\Users\Peter\AppData\Local\Temp\FastCopy\C2023-4-13-23:32:01"
	 */
	static std::wstring GetRecordFilePath(CopyOperation op);

};
