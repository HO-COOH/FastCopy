#pragma once
#include <winrt/base.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

template<typename Derived>
class IconBase
{
	winrt::hstring m_path;
public:
	winrt::hstring Path()
	{
		return m_path;
	}

	winrt::fire_and_forget Path(winrt::hstring const& value)
	{
		m_path = value;

		auto file = co_await Derived::getStorageItemFromPath(value);
		auto thumbnail = co_await file.GetThumbnailAsync(winrt::Windows::Storage::FileProperties::ThumbnailMode::SingleItem, 64);
		if (!thumbnail)
			co_return;

		winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
		co_await bitmap.SetSourceAsync(thumbnail);

		auto self = static_cast<Derived*>(this);
		self->Icon().Source(bitmap);
		self->IsLoading(false);
	}
};