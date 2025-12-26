#pragma once

#include "ConfirmDeleteFolderWindow.g.h"
#include "ConfirmWindowBase.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFolderWindow : ConfirmDeleteFolderWindowT<ConfirmDeleteFolderWindow>, ConfirmWindowBase<ConfirmDeleteFolderWindow>
    {
        ConfirmDeleteFolderWindow();

        winrt::hstring FolderName();

        constexpr static winrt::Windows::Graphics::SizeInt32 Size{ 545, 200 };
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
