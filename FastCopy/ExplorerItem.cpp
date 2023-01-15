#include "pch.h"
#include "ExplorerItem.h"
#if __has_include("ExplorerItem.g.cpp")
#include "ExplorerItem.g.cpp"
#endif

#include "UIDispatcher.h"
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.h>
#include "FileStats.h"

namespace winrt::FastCopy::implementation
{
	void ExplorerItem::recurseImpl()
	{
		UIDispatcher::g_dispatcher.TryEnqueue([this]()->winrt::Windows::Foundation::IAsyncAction
		{
			auto thisPtr = this;
			try
			{ 
				auto folder = co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(m_name);
				auto subFiles = co_await folder.GetFilesAsync();
				
				for (auto item : subFiles)
				{
					thisPtr->m_children.Append(winrt::FastCopy::ExplorerItem{ item });
				}

				auto subFolders = co_await folder.GetFoldersAsync();
				for (auto item : subFolders)
				{
					thisPtr->m_children.Append(winrt::FastCopy::ExplorerItem{ item, m_level > 0 - 1, m_level - 1 });
				}
			}
			catch(...)
			{
			}
		});
	}

	winrt::fire_and_forget ExplorerItem::calculateSize(winrt::Windows::Storage::IStorageItem item)
	{
		m_size = co_await FileStats::GetFolderSize(item);
		if (UIDispatcher::g_dispatcher)
			raisePropertyChange(L"Size");
	}

	winrt::fire_and_forget ExplorerItem::calculateSize(winrt::hstring path)
	{
		try {
			m_size = m_type == ExplorerItemType::File ?
				co_await FileStats::GetFolderSize(co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(path)) :
				co_await FileStats::GetFolderSize(co_await winrt::Windows::Storage::StorageFolder::GetFolderFromPathAsync(path));
			if (UIDispatcher::g_dispatcher)
				raisePropertyChange(L"Size");
		}
		catch (...)
		{

		}
	}

	ExplorerItem::ExplorerItem(winrt::hstring name, ExplorerItemType type) : m_type{ type }, m_name{ name }
	{
		calculateSize(name);
	}

	ExplorerItem::ExplorerItem(winrt::Windows::Storage::StorageFolder folder, bool recurse, int level) : m_name{folder.Path()}, m_type{FastCopy::ExplorerItemType::Folder}, m_level(level)
	{
		calculateSize(folder);
		if (level >= 0)
			recurseImpl();
	}

	void ExplorerItem::Type(ExplorerItemType type)
	{
		m_type = type;
		if (m_type == ExplorerItemType::File)
			return;

		if (!UIDispatcher::g_dispatcher || m_level < 0)
			return;

		recurseImpl();
	}

	void ExplorerItem::IsFailed(bool isFailed)
	{
		if (isFailed != m_isFailed)
		{
			m_isFailed = isFailed;
			raisePropertyChange(L"IsFailed");
		}
	}

	winrt::Windows::Foundation::IAsyncAction ExplorerItem::OpenLocation()
	{
		try
		{
			if (m_type == winrt::FastCopy::ExplorerItemType::File)
			{
				auto file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(m_name);
				co_await winrt::Windows::System::Launcher::LaunchFolderAsync(co_await file.GetParentAsync());
			}
			else
			{
				//folder
				co_await winrt::Windows::System::Launcher::LaunchFolderPathAsync(m_name);
			}
		}
		catch (...)
		{
			if (!UIDispatcher::g_dispatcher)
				co_return;
			UIDispatcher::g_dispatcher.TryEnqueue(
				[this]()->winrt::Windows::Foundation::IAsyncAction
				{
					winrt::Microsoft::UI::Xaml::Controls::ContentDialog dialog;
					dialog.Title(winrt::box_value(winrt::hstring{ L"Error: Cannot find file" }));
					dialog.CloseButtonText(L"OK");
					dialog.DefaultButton(winrt::Microsoft::UI::Xaml::Controls::ContentDialogButton::Close);
					dialog.XamlRoot(UIDispatcher::g_mainWindow.Content().as<winrt::Microsoft::UI::Xaml::Controls::Page>().XamlRoot());
					co_await dialog.ShowAsync();
				}
			);
		}
	}
}
