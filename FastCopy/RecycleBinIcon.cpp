#include "pch.h"
#include "RecycleBinIcon.h"
#include "ImageUtils.h"
#include <wil/resource.h>
#include <shellapi.h>
#include <CommCtrl.h>
#include <commoncontrols.h>

RecycleBinIcon::RecycleBinIcon() : WriteableBitmap
	{
		[]
		{
			SHSTOCKICONINFO stockIconInfo{.cbSize = sizeof(SHSTOCKICONINFO) };
			winrt::check_hresult(SHGetStockIconInfo(SIID_RECYCLERFULL, SHGSI_SYSICONINDEX, &stockIconInfo));

			winrt::com_ptr<IImageList> imageList;
			winrt::check_hresult(SHGetImageList(SHIL_JUMBO, IID_PPV_ARGS(imageList.put())));

			wil::unique_hicon icon;
			winrt::check_hresult(imageList->GetIcon(stockIconInfo.iSysImageIndex, ILD_TRANSPARENT, &icon));
			return HIconToWriteableBitmap(icon.get());
		}()
	}
{
}
