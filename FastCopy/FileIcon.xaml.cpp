#include "pch.h"
#include "FileIcon.xaml.h"
#if __has_include("FileIcon.g.cpp")
#include "FileIcon.g.cpp"
#endif
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{

    winrt::hstring FileIcon::Path()
    {
        return m_path;
    }

    winrt::fire_and_forget FileIcon::Path(winrt::hstring const& value)
    {
        m_path = value;
        auto file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(m_path);
        auto thumbnail = co_await file.GetThumbnailAsync(winrt::Windows::Storage::FileProperties::ThumbnailMode::SingleItem, 64);
        if (!thumbnail)
            co_return;

        winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmap;
        co_await bitmap.SetSourceAsync(thumbnail);
        Icon().Source(bitmap);

        IsLoading(false);
    }
}
