// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ProgressItem.g.h"
#include "ExplorerItem.g.h"

namespace winrt::FastCopy::implementation
{
    struct ProgressItem : ProgressItemT<ProgressItem>
    {
        ProgressItem();
        winrt::FastCopy::ExplorerItem ViewModel();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct ProgressItem : ProgressItemT<ProgressItem, implementation::ProgressItem>
    {
    };
}
