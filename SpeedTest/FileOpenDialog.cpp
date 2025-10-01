#include "FileOpenDialog.h"
#include <wil/result.h>

FileOpenDialog::FileOpenDialog()
{
	THROW_IF_FAILED(CoCreateInstance(
		CLSID_FileOpenDialog, 
		nullptr, 
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(m_ptr.put()))
	);
}

void FileOpenDialog::SetOptions(FILEOPENDIALOGOPTIONS options)
{
	THROW_IF_FAILED(m_ptr->SetOptions(options));
}

void FileOpenDialog::SetTitle(LPCWSTR pszTitle)
{
	THROW_IF_FAILED(m_ptr->SetTitle(pszTitle));
}

void FileOpenDialog::Show(HWND hwndOwner)
{
	THROW_IF_FAILED(m_ptr->Show(hwndOwner));
}

ShellItem FileOpenDialog::GetResult()
{
	IShellItem* item{};
	THROW_IF_FAILED(m_ptr->GetResult(&item));
	return ShellItem{ item };
}

ShellItem FileOpenDialog::PickSingleFolder(LPCWSTR title)
{
	FileOpenDialog dialog;
	dialog.SetOptions(FOS_PICKFOLDERS);
	if (title)
		dialog.SetTitle(title);
	dialog.Show();
	return ShellItem{ dialog.GetResult() };
}
