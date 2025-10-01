#pragma once
#include <wil/com.h>
#include <ShObjIdl_core.h>
#include "ShellItem.h"

class FileOpenDialog
{
	wil::com_ptr<IFileOpenDialog> m_ptr;
public:
	FileOpenDialog();
	void SetOptions(FILEOPENDIALOGOPTIONS options);
	void SetTitle(LPCWSTR pszTitle);
	/**
	 * @throw HRESULT_CANCELLED if user cancelled the dialog
	 */
	void Show(HWND hwndOwner = nullptr);
	ShellItem GetResult();

	static ShellItem PickSingleFolder(LPCWSTR title = nullptr);
};

