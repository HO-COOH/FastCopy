// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "CopyDialogWindow.g.h"
#include "AnimatedWindowSize.h"

namespace winrt::FastCopy::implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow>, AnimatedWindowSize
    {
        CopyDialogWindow();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialogWindow : CopyDialogWindowT<CopyDialogWindow, implementation::CopyDialogWindow>
    {
    };
}
