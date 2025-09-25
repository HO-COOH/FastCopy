#include "ExplorerGuard.h"
#include "Env.h"
#include "Process.h"

ExplorerGuard::ExplorerGuard()
{
	Env::Puts("Killing explorer.exe for accurate result. It will be restarted after test.\n");
	std::system("taskkill /f /im explorer.exe");
}

ExplorerGuard::~ExplorerGuard()
{
	Process<wchar_t> explorer{ Env::GetFolderPath(Env::SpecialFolder::Windows) + L"\\explorer.exe" };
}
