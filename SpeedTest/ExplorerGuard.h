#pragma once
/**
 * This class kills explorer.exe in constructor and then restart it in the destructor
 */
struct ExplorerGuard
{
	ExplorerGuard();
	~ExplorerGuard();

	ExplorerGuard(ExplorerGuard const&) = delete;
	ExplorerGuard(ExplorerGuard&&) = delete;
	ExplorerGuard& operator=(ExplorerGuard const&) = delete;
	ExplorerGuard& operator=(ExplorerGuard&&) = delete;
};