// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "ProgressItem.xaml.h"
#if __has_include("ProgressItem.g.cpp")
#include "ProgressItem.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FastCopy::implementation
{
    ProgressItem::ProgressItem()
    {
        InitializeComponent();
    }

    winrt::FastCopy::ExplorerItem ProgressItem::ViewModel()
    {
        return DataContext().as<winrt::FastCopy::ExplorerItem>();
    }

}
