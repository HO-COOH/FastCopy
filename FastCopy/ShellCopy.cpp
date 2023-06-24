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
		winrt::com_ptr<IFileOperation> pfo;
		auto hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(pfo.put()));

        if (!SUCCEEDED(hr))
            return false;

        if (!SUCCEEDED(pfo->SetOperationFlags(FOF_NO_UI)))
            return false;

        winrt::com_ptr<IShellItem> psiFrom, psiDest;
        std::wstring from = ToBackslash(source);
        if (hr = SHCreateItemFromParsingName(from.data(), NULL, IID_PPV_ARGS(psiFrom.put())); !SUCCEEDED(hr))
            return false;

        std::wstring to = ToBackslash(destination);
        if (hr = SHCreateItemFromParsingName(to.data(), NULL, IID_PPV_ARGS(psiDest.put())); !SUCCEEDED(hr))
            return false;

        if (!SUCCEEDED(pfo->MoveItem(psiFrom.get(), psiDest.get(), nullptr, nullptr)))
            return false;


        if (!SUCCEEDED(pfo->PerformOperations()))
            return false;

        return true;
	}
}
