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
#include <include/HwndHelper.hpp>

namespace winrt::FastCopy::implementation
{
    CopyDialogWindow::CopyDialogWindow() : AnimatedWindowSize{GetHwnd(*this)}
    {
        CenterWindow(*this, m_currentSize);
        ExtendsContentIntoTitleBar(true);
        Global::UIThread = DispatcherQueue();

        ViewModel().DuplicateFiles().VectorChanged(
            [this](winrt::Windows::Foundation::Collections::IObservableVector<winrt::FastCopy::FileCompareViewModel> original, auto&&)
            {
                auto const numElements = original.Size();
                auto const clamped = std::clamp<uint32_t>(numElements, 0, std::size(Sizes) - 1);

                if (m_currentSize == Sizes[clamped])
                    return;

                //set size cache and play window animation
                playWindowAnimation(m_currentSize = Sizes[clamped]);
            }
        );
        ViewModel().Finished(
            [this](winrt::Windows::Foundation::IInspectable const&, winrt::FastCopy::FinishState state)
            {
                if (state != winrt::FastCopy::FinishState::Failed)
                    return;

                //Some items failed: hide the progress dialog and show the (bound) error text instead
                Global::UIThread.TryEnqueue([this]
                {
                    CopyDialog().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
                    ErrorScrollViewer().Visibility(winrt::Microsoft::UI::Xaml::Visibility::Visible);
                    playWindowAnimation(Sizes[std::size(Sizes) - 1]);
                });
            }
        );

        Global::copyWindow = *this;
    }

    winrt::FastCopy::RobocopyViewModel CopyDialogWindow::ViewModel()
    {
        return ViewModelLocator::GetInstance().RobocopyViewModel();
    }


    void CopyDialogWindow::WindowEx_Closed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowEventArgs const& args)
    {
        ViewModelLocator::GetInstance().RobocopyViewModel().Cancel();
    }
}
