// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "CopyDialogWindow.xaml.h"
#if __has_include("CopyDialogWindow.g.cpp")
#include "CopyDialogWindow.g.cpp"
#endif

#include "WindowHelper.h"
#include "Global.h"
#include "Taskbar.h"
#include "ViewModelLocator.h"


namespace winrt::FastCopy::implementation
{
    CopyDialogWindow::CopyDialogWindow() : AnimatedWindowSize{GetHwnd(*this)}
    {
        CenterWindow(*this, m_currentSize);
        ExtendsContentIntoTitleBar(true);
        Global::UIThread = DispatcherQueue();

        ViewModel().DuplicateFiles().VectorChanged(
            [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto)
            {
                auto const numElements = original.Size();
                auto const clamped = std::clamp<uint32_t>(numElements, 0, std::size(Sizes) - 1);

                if (m_currentSize == Sizes[clamped])
                    return;

                //set size cache and play window animation
                playWindowAnimation(m_currentSize = Sizes[clamped]);
            }
        );
        Global::copyWindow = *this;
    }

    winrt::FastCopy::RobocopyViewModel CopyDialogWindow::ViewModel()
    {
        return ViewModelLocator::GetInstance().RobocopyViewModel();
    }
}

