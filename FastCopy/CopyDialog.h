#pragma once

#include "CopyDialog.g.h"
#include "RobocopyViewModel.g.h"

namespace winrt::FastCopy::implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog>
    {
        CopyDialog() 
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
            InitializeComponent();
        }

        winrt::FastCopy::RobocopyViewModel ViewModel();
    };
}

namespace winrt::FastCopy::factory_implementation
{
    struct CopyDialog : CopyDialogT<CopyDialog, implementation::CopyDialog>
    {
    };
}

