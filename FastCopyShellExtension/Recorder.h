#pragma once
#include <filesystem>
#include "CopyOperation.h"
#include "RecordFile.h"
class ShellItem;

class Recorder
{
public:
	Recorder(CopyOperation op);
	Recorder& operator<<(ShellItem& item);
	static bool HasRecord();
private:
	std::filesystem::path m_path;

	// Opened during construction, so the folder has to exist by the time GetRecordFilePath
	// returns - which is where it is created.
	RecordFile m_file{ m_path };

	/**
	 * Return the file name of the record file
	 * @example "C:\Users\Peter\AppData\Local\Temp\FastCopy\C2023-4-13-23:32:01"
	 */
	static std::filesystem::path GetRecordFilePath(CopyOperation op);

};
