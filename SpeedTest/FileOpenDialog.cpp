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

ShellItem FileOpenDialog::PickSingleFolder()
{
	FileOpenDialog dialog;
	dialog.SetOptions(FOS_PICKFOLDERS);
	dialog.Show();
	return ShellItem{ dialog.GetResult() };
}
