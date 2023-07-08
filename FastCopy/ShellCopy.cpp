#include "pch.h"
#include "ShellCopy.h"
#include <fileapi.h>
#include <ShObjIdl_core.h>
#include "PathUtils.h"

namespace ShellCopy
{
	bool IsInSameDiskPartition(std::wstring_view source, std::wstring_view destination)
	{
		wchar_t path[2][MAX_PATH]{};
		return GetVolumePathNameW(source.data(), path[0], MAX_PATH) &&
			GetVolumePathNameW(destination.data(), path[1], MAX_PATH) &&
			memcmp(path[0], path[1], sizeof(path[0])) == 0;
	}

	bool Move(std::wstring_view source, std::wstring_view destination)
	{
		try 
		{
			winrt::com_ptr<IFileOperation> pfo;
			winrt::check_hresult(CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(pfo.put())));
			winrt::check_hresult(pfo->SetOperationFlags(FOF_NO_UI));

			winrt::com_ptr<IShellItem> psiFrom = CreateItemFromParsingName(ToBackslash(source).data()),
				psiDest = CreateItemFromParsingName(ToBackslash(destination).data());
			winrt::check_hresult(pfo->MoveItem(psiFrom.get(), psiDest.get(), nullptr, nullptr));
			winrt::check_hresult(pfo->PerformOperations());
		}
		catch (...) { return false; }

        return true;
	}
}
