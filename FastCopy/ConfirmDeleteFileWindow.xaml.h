#pragma once

#include "ConfirmDeleteFileWindow.g.h"
#include "ConfirmWindowBase.h"

namespace winrt::FastCopy::implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow>, ConfirmWindowBase<ConfirmDeleteFileWindow>
    {
        ConfirmDeleteFileWindow();

        winrt::hstring const& FileName() const;
    private:
        winrt::hstring m_fileName;
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ConfirmDeleteFileWindow : ConfirmDeleteFileWindowT<ConfirmDeleteFileWindow, implementation::ConfirmDeleteFileWindow>
    {
    };
}
