#pragma once

#include "ConfirmDeleteFolderWindow.g.h"
#include "ConfirmWindowBase.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow>, ConfirmWindowBase<ConfirmDeleteFolderWindow>
    {
        ConfirmDeleteFolderWindow();

        winrt::hstring const& FolderName() const;
    private:
        winrt::hstring m_folderName;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow, implementation::ConfirmDeleteFolderWindow>
    {
    };
}
