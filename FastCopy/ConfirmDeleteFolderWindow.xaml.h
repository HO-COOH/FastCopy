#pragma once

#include "ConfirmDeleteFolderWindow.g.h"
#include "ConfirmWindowBase.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow>, ConfirmWindowBase<ConfirmDeleteFolderWindow>
    {
        ConfirmDeleteFolderWindow();

        winrt::hstring FolderName();
        winrt::hstring FolderPath();
        winrt::Windows::Foundation::DateTime DateCreated();

        constexpr static winrt::Windows::Graphics::SizeInt32 Size{ 545, 200 };
    private:
        winrt::hstring m_folderName;
        winrt::hstring m_folderPath;
        winrt::Windows::Foundation::DateTime m_dateCreated;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow, implementation::ConfirmDeleteFolderWindow>
    {
    };
}
